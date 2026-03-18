#include "buttons.h"

#include <avr/interrupt.h>

static volatile uint8_t debounce[BUTTON_COUNT] = {[0 ... BUTTON_COUNT-1] = DEBOUNCE_TIME}; //array {100,100,100}
volatile ButtonFlags buttonFlags = {0};

//needs to be called atomically
void debouncePass_ms(void){
    for(uint8_t i = 0; i < BUTTON_COUNT; i++){
        if(debounce[i] != 0) debounce[i]--;     
    }
}

/*-----------------
  BUTTON INTERRUPTS
------------------*/

//Button DISP
ISR(INT0_vect){
    if(debounce[0] == 0){
        debounce[0] = DEBOUNCE_TIME;
        buttonFlags.dispPressed = 1;
    }
}

//Buttons SET und DBG
ISR(PCINT0_vect){
  //SET-Button
    if(!(PINB & (1<<PB3)) && debounce[1] == 0){
        debounce[1] = DEBOUNCE_TIME;
        buttonFlags.setPressed = 1;
    }

  //DBG-Button
    if(!(PINB & (1<<PB4)) && debounce[2] == 0){
        debounce[2] = DEBOUNCE_TIME;
        buttonFlags.dbgPressed = 1;;
    }
}