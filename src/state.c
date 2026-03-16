#include "state.h"

#include "driver.h"
#include "time.h"
#include <avr/interrupt.h>

volatile State state = INIT;


/*----------------
STATE CHANGE LOGIC
----------------*/

void initialized(void){
    if(state == INIT) state = SHOW_MINUTES; 
}

void toggleMinuteMode(void){
    if(state == SHOW_MINUTES){
        setLeds(time.hours,time.seconds);
        state = SHOW_SECONDS;
    }
    else if(state == SHOW_SECONDS){
        setLeds(time.hours,time.minutes);
        state = SHOW_MINUTES;
    }
}

void toggleDbg(){
    if(state == SHOW_MINUTES || state == SHOW_SECONDS){
        state = DBG;
    }
    else if(state == DBG){
        state = SHOW_MINUTES;
    }
}

void enterSetupMode(void) {
    if(state == SHOW_MINUTES || state == SHOW_SECONDS) {
        state = SETUP_HOURS;
    }
}

void continueSetup(void){
    if(state == SETUP_HOURS){
        state = SETUP_MINUTES;
    }
}

void finishSetup(void){
    if(state == SETUP_MINUTES){
        state = SHOW_MINUTES;
    }
}




/*------------
STATE LOGIC
------------*/

void initState(void) {
    initLeds();
    initButtons();
    initPwm();
    initQuartz();
    setLeds(time.hours, time.minutes);
    sei();

    initialized();
}

void showMinutesState(void){
    //logic in time.c
}

void showSecondsState(void){
    //logic in time.c 
}

void setupHoursState(void) {

}
void setupMinutesState(void) {
    
}

void debugState(void){

}