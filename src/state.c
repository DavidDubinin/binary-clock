#include "state.h"

#include "driver.h"
#include "time.h"
#include "buttons.h"
#include <avr/interrupt.h>

#define UP 0
#define DOWN 1

volatile State state = INIT;

/*----------------
STATE CHANGE LOGIC
----------------*/

static void count(uint8_t up){
    Time newTime = getTime();
    newTime.seconds = 0;
    TimeFlags newFlags = {0};

    if(state == SETUP_HOURS) {
        newFlags.hourPassed = 1;
    }
    else if(state == SETUP_MINUTES) {
        newFlags.minutePassed = 1;
    }

    incrementTimeSetup_reversible(&newTime, &newFlags, up);   
    setLeds(newTime.hours, newTime.minutes);
    setTime(&newTime);
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
    state = SHOW_MINUTES;
}

void showMinutesState(void){
    //toggleMinutesMode
    if(buttonFlags.dispPressed){
        buttonFlags.dispPressed = 0;
        setLeds(getTime().hours,getTime().seconds);
        state = SHOW_SECONDS;
    }

    //enterSetupMode
    if(buttonFlags.setPressed) {
        buttonFlags.setPressed = 0;
        state = SETUP_HOURS;
    }

    //toggleDbg
    if(buttonFlags.dbgPressed){
        buttonFlags.dbgPressed = 0;
        state = DBG;
    }

}

void showSecondsState(void){
    //toggleMinutesMode
    if(buttonFlags.dispPressed){
        buttonFlags.dispPressed = 0;
        setLeds(getTime().hours,getTime().minutes);
        state = SHOW_MINUTES;
    }

    //enterSetupMode
    if(buttonFlags.setPressed) {
        buttonFlags.setPressed = 0;
        state = SETUP_HOURS;
    }

    //toggleDbg
    if(buttonFlags.dbgPressed){
        buttonFlags.dbgPressed = 0;
        state = DBG;
    }
}

void setupHoursState(void) {
    //continueSetup
    if(buttonFlags.dispPressed){
        buttonFlags.dispPressed = 0;
        state = SETUP_MINUTES;
    }
    //countUp
    if(buttonFlags.setPressed){
        buttonFlags.setPressed = 0;
        count(UP);
    }
    //countDown
    if(buttonFlags.dbgPressed){
        buttonFlags.dbgPressed = 0;
        count(DOWN);
    }

}

void setupMinutesState(void) {
    //finishSetup
    if(buttonFlags.dispPressed){
        buttonFlags.dispPressed = 0;
        state = SHOW_MINUTES;
    }
    //countUp
    if(buttonFlags.setPressed){
        buttonFlags.setPressed = 0;
        count(UP);
    }
    //countDown
    if(buttonFlags.dbgPressed){
        buttonFlags.dbgPressed = 0;
        count(DOWN);
    }
}

void debugState(void){
    //toggleDbg
    if(buttonFlags.dbgPressed){
        buttonFlags.dbgPressed = 0;
        state = SHOW_MINUTES;
    }
}