#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define DEBOUNCE_TIME 100 //in ms
#define BUTTON_COUNT 3
#define DUTY_CYCLE 1 // in %

#define hour 17
#define minute 59
#define sec 25

volatile uint32_t time = (uint32_t)hour*60*60 + minute*60 + sec; // unit 1s //0 bis 24*60*60 = 86 400 (86 399 -> 0)
volatile uint8_t minutesMode = 1;
volatile uint8_t debounce[BUTTON_COUNT] = {[0 ... BUTTON_COUNT-1] = DEBOUNCE_TIME}; //array {100,100,100}

typedef enum {
  INIT,
  RUNNING,
  DISP,
  SET,
  DBG
} State;

typedef enum {
  OFF,
  HOURS,
  MINUTES
} SetupMode;

typedef struct {
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
} Time;

volatile State state = INIT;
volatile SetupMode setupMode = OFF;

Time calcTime(void){
  Time timeStruct;
  
  timeStruct.hours = time / (3600); // 000_0 1101 Stunden
  timeStruct.minutes = (time % (3600)) / 60; // 00_00 1110 Minuten
  timeStruct.seconds = time % 60;  //0000
  return timeStruct;
}

void setLeds(Time timeStruct) {
  //^need to be reversed
  PORTD = __builtin_avr_insert_bits(0x01234FFF, timeStruct.hours, PORTD); // portd 7 bis 3  stunden
  if (minutesMode) PORTC = __builtin_avr_insert_bits(0xFF012345, timeStruct.minutes, PORTC); //portc 5 bis 0 minuten
  else PORTC = __builtin_avr_insert_bits(0xFF012345, timeStruct.seconds, PORTC); //portc 5 bis 0 Sekunden
}

void blinkendeStunden(void) {
  while(1) {
    PORTD |= (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7);
    _delay_ms(500);
    PORTD &= ~(1 << PD3) & ~(1 << PD4) & ~(1 << PD5) & ~(1 << PD6) & ~(1 << PD7);
    _delay_ms(500);
  }
}

void blinkendeMinuten(void) {
  while(1) {
    PORTC |= (1 << PC0) | (1 << PC1) | (1 << PC2) | (1 << PC3) | (1 << PC4) | (1 << PC5);
    _delay_ms(500);
    PORTC &=  ~(1 << PC0) & ~(1 << PC1) & ~(1 << PC2) & ~(1 << PC3) & ~(1 << PC4) & ~(1 << PC5);
    _delay_ms(500);
  }
}

//time = 86 398 -> 86 399 ->  86 400 mod 86 400 => 0
void incrementTime(void) { 
  time = (time + 1) % 86400;

  //update disp jede Minute wenn in Minutenmodus oder jede Sekunde wenn nicht
  if(!minutesMode || time % 60 == 0){
    setLeds(calcTime());
  }
}

void initLeds(void) {
  DDRB |= (1 << DDB1) | (1 << DDB2); // OC1A/PB1 bzw OC1B/PB2 auf Output für Stunden/Minuten
  DDRD |= (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7); // Stundenpins, 5 bit
  DDRC |= (1 << PC0) | (1 << PC1) | (1 << PC2) | (1 << PC3) | (1 << PC4) | (1 << PC5); // Minutenpins, 6 bit
}

// Set OC1A/OC1B on Compare Match, clear OC1A/OC1B at TOP 
// ; Fast PWM, 10-bit, TOP = 0x03FF, Update of OCR1x at TOP, TOV1 Flag set on TOP
// CS: clkI/O/1 (No prescaling) 
void initPwm(void) {
  TCCR1A |= (1 << WGM10) | (1 << WGM11) | (1 << COM1A1) 
         | (1 << COM1B1) | (1 << COM1A0) | (1 << COM1B0);
         
  TCCR1B |= (1 << WGM12) | (1 << CS10);
  TCCR1B &= ~(1 << WGM13) & ~(1 << CS11) & ~(1 << CS12);

  OCR1A = DUTY_CYCLE*1023/100;
  OCR1B = DUTY_CYCLE*1023/100;
  
}

//Watch Crystal 32 768 Hz, Prescaler 128, F_OCnx = 1/2 Hz (1 interrupt per sec) => OCRnx = 255  
//Toggle OC2A on Compare Match, CTC Mode, TOP=OCR2A

//Watch Crystal 32 768 Hz, Prescaler 128, F_OCnx = 500 Hz (1 interrupt each ms) => OCRnx = 255  
//Toggle OC2A on Compare Match, CTC Mode, TOP=OCR2A
void initQuartz(void){
  //1. Disable the Timer/Counter2 interrupts by clearing OCIE2x and TOIE2
  TIMSK2 &= ~(1<<OCIE2A) & ~(1<<OCIE2B) & ~(1<<TOIE2);

  //2. Select clock source by setting AS2 as appropriate
  //Enable Asynchronous Timer/Counter2
  ASSR |= (1 << AS2);

  //3. Write new values to TCNT2, OCR2x, and TCCR2x.
  //Toggle OC2A on Compare Match

  TCNT2 = 0x00; //reset timer
  TCCR2A |= (1 << COM2A0);
  TCCR2A &= ~(1 << COM2A1);

  //CTC-Mode
  TCCR2A |= (1 << WGM21);
  TCCR2A &= ~(1 << WGM20);
  TCCR2B &= ~(1 << WGM22);
  
  //CS selection mit Prescaler = 128
  TCCR2B |= (1 << CS20) | (1 << CS22);
  TCCR2B &= ~(1 << CS21);

  //Compare Match bei Timer = 255 => OC2A Interrupt Flag Set
  OCR2A = 0xFF;

  //4. To switch to asynchronous operation: Wait for TCN2xUB, OCR2xUB, and TCR2xUB
  //ASSR = xxx1 1111 BAD
  //ASSR = xxx0 0000
  while(ASSR & ((1<<TCN2UB) | (1<<OCR2AUB) | (1<<OCR2BUB) | (1<<TCR2AUB) | (1<<TCR2BUB))){
    __builtin_avr_nop(); // sigma approved!!!
  }

  //5. Clear the Timer/Counter2 Interrupt Flags
  TIFR2 |= (1 << OCF2B) | (1 << OCF2A) | (1 << TOV2);

  //6. Enable interrupts, if needed
  //Enable Output Compare Match Interrupt A
  TIMSK2 |= (1 << OCIE2A);
}

void initButtons(void) {
  //set Button pins as inputs
  DDRD &= ~(1 << PD2);
  DDRB &= ~(1 << PB3) & ~(1 << PB4);
  //enable internal pullups for Buttons
  PORTD |= (1 << PD2);
  PORTB |= (1 << PB3) | (1 << PB4);

  //set INT0 trigger to falling edge
  EICRA |= (1 << ISC00);
  EICRA &= ~(1 << ISC01);
  //Enable INT0
  EIMSK |= (1 << INT0);

  //Enable PCINT7..0 interrupt und Maske for PCINT3..4
  PCICR |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT3) | (1 << PCINT4);
}

void init(void) {
  initLeds();
  initButtons();
  initPwm();
  initQuartz();
  setLeds(calcTime());
  sei();
}

int main(void){
  while(1){
    switch (state){
    case INIT:
      init();

      state = RUNNING;
      break;

    case RUNNING:
      if(debounce[0]) debounce[0] -= 1;
      if(debounce[1]) debounce[1] -= 1;
      if(debounce[2]) debounce[2] -= 1;

      _delay_ms(1);
      break;

    case DISP:
    if(setupMode == OFF){
      minutesMode = !minutesMode;
      setLeds(calcTime());
    }
    else if(setupMode == HOURS){
      setupMode = MINUTES;
    }
    else if(setupMode == MINUTES){
      setupMode = OFF;
    }
      state = RUNNING;
      break;
      
    case SET:
      if(setupMode == OFF){
        setupMode = HOURS;
      }
      state = RUNNING;
      break;

    case DBG:
      state = RUNNING;
      break;

    default:
      cli();
      return 0;
    }
  }
}

ISR(TIMER2_COMPA_vect){
  incrementTime();
}

//Button DISP
ISR(INT0_vect){
  if(debounce[0] == 0){state = DISP; debounce[0] = DEBOUNCE_TIME;}
}

//Buttons SET und DBG
ISR(PCINT0_vect){
  //SET-Button
  if(!(PINB & (1<<PB3))){
    if(debounce[1] == 0){state = SET; debounce[1] = DEBOUNCE_TIME;}
  }
  //DBG-Button
  if(!(PINB & (1<<PB4))){
    if((debounce[2] == 0)){state = DBG; debounce[2] = DEBOUNCE_TIME;}
  }
}