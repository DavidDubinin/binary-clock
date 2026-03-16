#include "setup_mode.h"

volatile SetupMode setupMode = OFF;
volatile uint16_t blinkTimer = BLINK_TIMER;

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