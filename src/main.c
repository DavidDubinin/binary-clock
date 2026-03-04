#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


#define DUTY_CYCLE 1 // in %
#define hour 20
#define minute 31
#define sec 30

volatile uint32_t time = (uint32_t)hour*60*60 + minute*60 + sec; // unit 1s //0 bis 24*60*60 = 86 400 (86 399 -> 0)

void setLeds(uint32_t time) { 
  uint8_t hours = time / (3600); // 000_0 1101 Stunden
  uint8_t minutes = (time % (3600)) / 60; // 00_00 1110 Minuten
  //^need to be reversed
  PORTD = __builtin_avr_insert_bits(0x01234FFF, hours, PORTD); // portd 7 bis 3  stunden
  PORTC = __builtin_avr_insert_bits(0xFF012345, minutes, PORTC); //portc 5 bis 0 minuten
} 

//time = 86 398 -> 86 399 ->  86 400 mod 86 400 => 0
void incrementTime(void) { 
  time = (time + 1) % 86400;

  //update disp
  if(time % 60 == 0){
    setLeds(time);
  }
}

void initPins(void) {
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
  while(ASSR & ( (1<<TCN2UB) | (1<<OCR2AUB) | (1<<OCR2BUB) | (1<<TCR2AUB) | (1<<TCR2BUB))){
    __builtin_avr_nop(); // sigma approved!!!
  }

  //5. Clear the Timer/Counter2 Interrupt Flags
  TIFR2 |= (1 << OCF2B) | (1 << OCF2A) | (1 << TOV2);

  //6. Enable interrupts, if needed
  //Enable Output Compare Match Interrupt A
  TIMSK2 |= (1 << OCIE2A);
}

int main(void){
  initPins();
  initPwm();
  initQuartz();
  sei();
  
  setLeds(time);

  while(1){
    __builtin_avr_nop();
  }
  return 0;
}

ISR(TIMER2_COMPA_vect){
  incrementTime();
}