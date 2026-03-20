#include "buttons.h"//button interrupts
#include "time.h"//1ms interrupt
#include "uart.h"//RX interrupt
#include "state.h"

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
        state = INIT;
        break;
    }
  }
}