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
        setLeds(getTime().hours,getTime().seconds);
        state = SHOW_SECONDS;
    }
    else if(state == SHOW_SECONDS){
        setLeds(getTime().hours,getTime().minutes);
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

void countUp(void) {
    if(state == SETUP_HOURS || state == SETUP_MINUTES) {
        Time newTime = getTime();
        TimeFlags newFlags = {0};

        if(state == SETUP_HOURS) {
            newFlags.hourPassed = 1;
        }
        else {
            newFlags.minutePassed = 1;
        }

        incrementTimeStruct(&newTime, &newFlags);
        newTime.seconds = 0;
        setTime(&newTime);
        setLeds(newTime.hours, newTime.minutes);
    }
}

void countDown(void) {
    if(state == SETUP_HOURS || state == SETUP_MINUTES) {
        Time newTime = getTime();
        TimeFlags newFlags = {0};

        if(state == SETUP_HOURS) {
            newFlags.hourPassed = 1;
        }
        else {
            newFlags.minutePassed = 1;
        }

        decrementTimeStruct(&newTime, &newFlags);
        newTime.seconds = 0;
        setTime(&newTime);
        setLeds(newTime.hours, newTime.minutes);
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
    setLeds(getTime().hours, getTime().minutes);
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