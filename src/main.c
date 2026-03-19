#include "buttons.h"
#include "time.h"
#include "state.h"
#include "sleepTimer.h"
#include <avr/sleep.h>

int main(void){
    set_sleep_mode(SLEEP_MODE_PWR_SAVE); // Prozessor Schläft, TIMER2 arbeitet weiter, ext Interrupt weckt es auf
    sleep_enable(); //noch keine interrupts
    //sleep_disable();

  while(1){
    trySleep();
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