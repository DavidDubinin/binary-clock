#define F_CPU 1000000UL


#include "driver.h"
#include "time.h"

#include <util/delay.h>
#include <avr/interrupt.h>

void init(void) {
  initLeds();
  initButtons();
  initPwm();
  initQuartz();
  setLeds(time.hours, time.minutes);
  sei();
}

int main(void){
  init();

  while(1){
    asm("nop");
  }

  cli();
  return 0;
}



/*
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
*/