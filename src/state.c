#include "state.h"

#include "driver.h"//alles
#include "buttons.h"//buttonFlags
#include "time.h"//Time types
#include "sleepTimer.h"//configMinimalPower()
#include "uart.h"

#include <avr/interrupt.h>
#include <util/atomic.h>

#define UP 0
#define DOWN 1

volatile State state = INIT;
Time newTime;

/*----------------
STATE CHANGE LOGIC
----------------*/

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
    uint8_t up = 0;
    uint8_t down = 0;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    //countUp
        if(buttonFlags.setPressed){
            buttonFlags.setPressed = 0;
            up = 1;
        }

    //countDown
        if(buttonFlags.dbgPressed){
            buttonFlags.dbgPressed = 0;
            down = 1;
        }
    }
    
    if(up){
        count_reversible(UP);
    }
    
    else if(down){
        count_reversible(DOWN);
    }
}




/*------------
STATE LOGIC
------------*/

void initState(void) {
    cli();
    initLeds();
    initButtons();
    initPwm();
    initQuartz_ms();
    configMinimalPower();
    initUart();

    Time t = getTime();
    setLeds(t.hours, t.minutes);

    state = SHOW_MINUTES;
    sei();
}

void showMinutesState(void){
    trySleep();
    uint8_t dispPressed = 0;
    uint8_t setPressed = 0;
    uint8_t dbgPressed = 0;
    uint8_t minPassed = 0;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        //toggleMinutesMode
        if(buttonFlags.dispPressed){
            buttonFlags.dispPressed = 0;
            dispPressed = 1;
        }

        //enterSetup
        if(buttonFlags.setPressed) {
            buttonFlags.setPressed = 0;
            setPressed = 1;
        }
        
        //update Leds 
        if(timeFlags_external.minutePassed){
            timeFlags_external.minutePassed = 0;
            minPassed = 1;
        }

        //enterDbg
        if(buttonFlags.dbgPressed) {
            buttonFlags.dbgPressed = 0;
            dbgPressed = 1;
        }
    }

    if(setPressed){
        newTime = getTime();
        state = SETUP_HOURS;
        return;
    }
    

    else if(dispPressed) {
        Time t = getTime();
        setLeds(t.hours,t.seconds);
        state = SHOW_SECONDS;
        return;
    }

    else if(dbgPressed){
        state = DBG;
        return;
    };

    if(minPassed){
        Time t = getTime();
        setLeds(t.hours, t.minutes);
    }
}

void showSecondsState(void){
    trySleep();
    uint8_t dispPressed = 0;
    uint8_t setPressed = 0;
    uint8_t dbgPressed = 0;
    uint8_t secPassed = 0;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        //toggleMinutesMode
        if(buttonFlags.dispPressed){
            buttonFlags.dispPressed = 0;
            dispPressed = 1;
        }

        //enterSetup
        if(buttonFlags.setPressed) {
            buttonFlags.setPressed = 0;
            setPressed = 1;
        }
        
        //update Leds 
        if(timeFlags_external.secondPassed){
            timeFlags_external.secondPassed = 0;
            secPassed = 1;
        }

        //enterDbg
        if(buttonFlags.dbgPressed) {
            buttonFlags.dbgPressed = 0;
            dbgPressed = 1;
        }
    }

    if(setPressed){
        newTime = getTime();
        state = SETUP_HOURS;
        return;
    }
    

    else if(dispPressed) {
        Time t = getTime();
        setLeds(t.hours,t.minutes);
        state = SHOW_MINUTES;
        return;
    }

    else if(dbgPressed){
        state = DBG;
        return;
    }

    if(secPassed){
        Time t = getTime();
        setLeds(t.hours, t.seconds);
    }
}

void setupHoursState(void) {
    trySleep();
    //continueSetup
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if(buttonFlags.dispPressed){
            buttonFlags.dispPressed = 0;
            state = SETUP_MINUTES;
            return;
        }
    }

    count();
}

void setupMinutesState(void) {
    trySleep();
    uint8_t dispPressed = 0;
    //finishSetup
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if(buttonFlags.dispPressed){
            buttonFlags.dispPressed = 0;
            dispPressed = 1;
        }
    }
    
    if(dispPressed) {
        setTime(&newTime);
        state = SHOW_MINUTES;
        return;
    }
    
    count();
}

void debugState(void){
    //toggleDbg
    uint8_t dbgPressed = 0;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if(buttonFlags.dbgPressed){
            buttonFlags.dbgPressed = 0;
            dbgPressed = 1;
        }
    }
    if(dbgPressed) {
        Time t = getTime();
        setLeds(t.hours, t.minutes);
        state = SHOW_MINUTES;
        return;
    }
    processUart();

}