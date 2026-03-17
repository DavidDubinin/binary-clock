#define F_CPU 1000000UL

#include "buttons.h"
#include "time.h"
#include "state.h"

#include <util/delay.h>
#include <avr/interrupt.h>

int main(void){
  
  while(1){
    switch (state) {
      case INIT:
        initState();
        break;
      
      case SHOW_MINUTES:
        showMinutesState();
        break;

      case SHOW_SECONDS:
        showSecondsState();
        break;

      case SETUP_HOURS:
        setupHoursState();
        break;

      case SETUP_MINUTES:
        setupMinutesState();
        break;
      
      case DBG:
        debugState();
        break;
      
      default:
        return 0;
    }
  }
}