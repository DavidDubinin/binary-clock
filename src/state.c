#include "state.h"

#include "driver.h"
#include "buttons.h"
#include "time.h"

#include <avr/interrupt.h>

#define UP 0
#define DOWN 1

volatile State state = INIT;
Time newTime;

/*----------------
STATE CHANGE LOGIC
----------------*/

static void enterSetupMode(void){
    if(buttonFlags.setPressed) {
        buttonFlags.setPressed = 0;
        newTime = getTime();
        state = SETUP_HOURS;
    }
}

static void enterDbg(void){
    if(buttonFlags.dbgPressed){
        buttonFlags.dbgPressed = 0;
        state = DBG;
    }
}

static void count_reversible(uint8_t up){
    newTime.seconds = 0;
    newTime.milliseconds = 0;
    TimeFlags newFlags = {0};

    if(state == SETUP_HOURS) {
        newFlags.hourPassed = 1;
    }
    else if(state == SETUP_MINUTES) {
        newFlags.minutePassed = 1; 
    }
    incrementTimeSetup_reversible(&newTime, &newFlags, up);   
    setLeds(newTime.hours, newTime.minutes);
}

static void count(void){
    //countUp
    if(buttonFlags.setPressed){
        buttonFlags.setPressed = 0;
        count_reversible(UP);
    }
    //countDown
    if(buttonFlags.dbgPressed){
        buttonFlags.dbgPressed = 0;
        count_reversible(DOWN);
    }
}




/*------------
STATE LOGIC
------------*/

void initState(void) {
    initLeds();
    initButtons();
    initPwm();
    initQuartz_ms();
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

    if(timeFlags_external.minutePassed){
        timeFlags_external.minutePassed = 0;
        setLeds(getTime().hours, getTime().minutes);
    }

    enterSetupMode();
    enterDbg();

}

void showSecondsState(void){
    //toggleMinutesMode
    if(buttonFlags.dispPressed){
        buttonFlags.dispPressed = 0;
        setLeds(getTime().hours,getTime().minutes);
        state = SHOW_MINUTES;
    }

    if(timeFlags_external.secondPassed){
        timeFlags_external.secondPassed = 0;
        setLeds(getTime().hours, getTime().seconds);
    }
    enterSetupMode();
    enterDbg();

}

void setupHoursState(void) {
    //continueSetup
    if(buttonFlags.dispPressed){
        buttonFlags.dispPressed = 0;
        state = SETUP_MINUTES;
    }
    count();
}

void setupMinutesState(void) {
    //finishSetup
    if(buttonFlags.dispPressed){
        buttonFlags.dispPressed = 0;
        setTime(&newTime);
        state = SHOW_MINUTES;
    }
    count();
}

void debugState(void){
    //toggleDbg
    if(buttonFlags.dbgPressed){
        buttonFlags.dbgPressed = 0;
        setLeds(getTime().hours, getTime().minutes);
        state = SHOW_MINUTES;
    }
}