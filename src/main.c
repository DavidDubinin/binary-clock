#define F_CPU 1000000UL


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define DEBOUNCE_TIME 100 //in ms
#define BUTTON_COUNT 3
#define BLINK_TIMER 1000 //in ms (T)



volatile uint8_t debounce[BUTTON_COUNT] = {[0 ... BUTTON_COUNT-1] = DEBOUNCE_TIME}; //array {100,100,100}

volatile uint16_t blinkTimer = BLINK_TIMER;

volatile State state = INIT;
volatile SetupMode setupMode = OFF;




/*
void blinkendeMinuten(void){
    if(blinkTimer > (BLINK_TIMER/2)) {
      PORTC &=  ~(1 << PC0) & ~(1 << PC1) & ~(1 << PC2) & ~(1 << PC3) & ~(1 << PC4) & ~(1 << PC5);
    }
    else {
      PORTC |= (1 << PC0) | (1 << PC1) | (1 << PC2) | (1 << PC3) | (1 << PC4) | (1 << PC5);
    }
    if(blinkTimer == 0) blinkTimer = BLINK_TIMER;
}

void blinkendeStunden(void){
    if(blinkTimer > (BLINK_TIMER/2)) {
      PORTD &= ~(1 << PD3) & ~(1 << PD4) & ~(1 << PD5) & ~(1 << PD6) & ~(1 << PD7);
    }
    else {
      PORTD |= (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7);
    }
    if(blinkTimer == 0) blinkTimer = BLINK_TIMER;
}


void blinkendeLampen(void) {
  if (setupMode == HOURS) {
    blinkendeMinuten();
    
  }
  else if(setupMode == MINUTES) {
    blinkendeStunden();
  }
}
*/



void init(void) {
  initLeds();
  initButtons();
  initPwm();
  initQuartz();
  setLeds(time.hours, time.minutes);
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
      _delay_ms(1);
      if(debounce[0]) debounce[0] -= 1;
      if(debounce[1]) debounce[1] -= 1;
      if(debounce[2]) debounce[2] -= 1;

      
      if(setupMode != OFF && blinkTimer != 0) {
        blinkTimer -= 1;
      }

      blinkendeLampen();

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