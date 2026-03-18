#include "time.h"

#include "buttons.h"
#include <avr/interrupt.h>
#include <util/atomic.h>

volatile static Time time = {0};
volatile static TimeFlags timeFlags = {0};

volatile TimeFlags timeFlags_external = {0};


static inline uint8_t timeIsValid(Time t){
    return  t.milliseconds < 1000 && t.seconds < 60 && t.minutes < 60 && t.hours < 24;
}

static void updateSeconds(volatile Time* t, volatile TimeFlags* tflags, uint8_t backwards){
    if(!backwards){
        if (t->seconds == 59){
            t->seconds = 0;
            tflags->minutePassed = 1;
        }
        else{
            t->seconds++;
        } 
    }
    else{
        if (t->seconds == 0){
            t->seconds = 59;
            tflags->minutePassed = 1;
        }
        else{
            t->seconds--;
        }
    }
}

static void updateMinutes(volatile Time* t, volatile TimeFlags* tflags, uint8_t backwards){
    if(!backwards){
        if (t->minutes == 59){
            t->minutes = 0;
            tflags->hourPassed = 1;
        }
        else{
            t->minutes++;
        }    
    }
    else {
        if (t->minutes == 0){
            t->minutes = 59;
            tflags->hourPassed = 1;
    }
        else {
            t->minutes--;
        }
    }
}

static void updateHours(volatile Time* t, volatile TimeFlags* tflags, uint8_t backwards){
    if(!backwards){
        if (t->hours == 23){
            t->hours = 0;
        }
        else{
            t->hours++;
        }
    }
    else{
        if (t->hours == 0){
            t->hours = 23;
        }
        else{
            t->hours--;
        }
    }
}

static void incrementTime_reversible(volatile Time* t, volatile TimeFlags* tflags, uint8_t backwards) {
    if(tflags->secondPassed) {
        tflags->secondPassed = 0;
        updateSeconds(t, tflags, backwards);
        timeFlags_external.secondPassed = 1;
    }
    
    if(tflags->minutePassed) {
        tflags->minutePassed = 0;
        updateMinutes(t, tflags, backwards);
        timeFlags_external.minutePassed = 1;
    }

    if(tflags->hourPassed) {
        tflags->hourPassed = 0;
        updateHours(t, tflags, backwards);
    }
}

static void tick_ms(volatile Time* t, volatile TimeFlags* tflags, uint8_t backwards){
    if(t->milliseconds == 999){
        t->milliseconds = 0;
        tflags->secondPassed = 1;
        incrementTime_reversible(t, tflags, 0);
    }
    else{
        t->milliseconds++;
    }
}

void incrementTimeSetup_reversible(Time* newTime, TimeFlags* newTflags, uint8_t backwards){
    TimeFlags dummy = {0}; //Alphawolf approved !!
    if(newTflags->secondPassed) {
        newTflags->secondPassed = 0;
        updateSeconds(newTime, &dummy, backwards);
    }
    
    if(newTflags->minutePassed) {
        newTflags->minutePassed = 0;
        updateMinutes(newTime, &dummy, backwards);
    }

    if(newTflags->hourPassed) {
        newTflags->hourPassed = 0;
        updateHours(newTime, &dummy, backwards);
    }
}

int setTime(Time* newTime){
        if(timeIsValid(*newTime)){
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
                time = *newTime;
                timeFlags.hourPassed = 0;
                timeFlags.minutePassed = 0;
                timeFlags.secondPassed = 0;
            }
            return 0;
        }
    return -1;
}


Time getTime(void){
    Time t;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        t = time;
    }
    return t;
}

//Diskret: Alle 128 Interrupts 3ms entfernen. Alternativ, stetig: Bresenham Algorithmus (hier)
ISR(TIMER2_COMPA_vect){ //called every ms
    static uint16_t acc = 0;
    acc += 1000;

    if(acc >= 1024){
        acc -= 1024;
        debouncePass_ms();
        tick_ms(&time, &timeFlags, 0);
    }
}