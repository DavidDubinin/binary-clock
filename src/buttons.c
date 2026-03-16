#include "buttons.h"
#include "time.h"
#include <avr/interrupt.h>

volatile uint8_t debounce[BUTTON_COUNT] = {[0 ... BUTTON_COUNT-1] = DEBOUNCE_TIME}; //array {100,100,100}

static inline uint8_t min(uint8_t val1, uint8_t val2){
    return val1 < val2 ? val1 : val2;
}

void debouncePass(uint8_t ms){
    for(uint8_t i = 0; i < BUTTON_COUNT; i++){
        if(debounce[i] != 0) debounce[i] -= min(debounce[i],ms);
    }
}


//Button DISP
ISR(INT0_vect){
    if(debounce[0] == 0){
        minutesMode = !minutesMode;
        setLeds(time.hours, time.minutes);
        debounce[0] = DEBOUNCE_TIME;
    }
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