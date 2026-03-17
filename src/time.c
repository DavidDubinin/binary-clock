#include "time.h"

#include "driver.h"
#include "state.h"
#include "buttons.h"

#include <avr/interrupt.h>

volatile static Time time = {0};
volatile static TimeFlags timeFlags = {0};
volatile static uint16_t ms = 0;

static inline uint8_t timeIsValid(Time t){
    return t.seconds < 60 && t.minutes < 60 && t.hours < 24;
}

static void tick_ms(TimeFlags* tflags, uint8_t backwards){
    if(ms == 999){
        ms = 0;
        tflags->secondPassed = 1;
    }
    else{
        ms++;
    }
}

static void updateSeconds(Time* t, TimeFlags* tflags, uint8_t backwards){
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

static void updateMinutes(Time* t, TimeFlags* tflags, uint8_t backwards){
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

static void updateHours(Time* t,TimeFlags* tflags, uint8_t backwards){
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

static void incrementTime_reversible(Time* t, TimeFlags* tflags, uint8_t backwards) {
    if(tflags->secondPassed) {
        tflags->secondPassed = 0;
        updateSeconds(t, tflags, backwards);
        if(state == SHOW_SECONDS) setLeds(t->hours,t->seconds);
    }
    
    if(tflags->minutePassed) {
        tflags->minutePassed = 0;
        updateMinutes(t, tflags, backwards);
        if(state == SHOW_MINUTES) setLeds(t->hours,t->minutes);
    }

    if(tflags->hourPassed) {
        tflags->hourPassed = 0;
        updateHours(t, tflags, backwards);
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
        time = *newTime;
        timeFlags.hourPassed = 0;
        timeFlags.minutePassed = 0;
        timeFlags.secondPassed = 0;
        return 0;
    }
    return -1;
}

Time getTime(){
    return time;
}
/*
ISR(TIMER2_COMPA_vect){ //called every second
    timeFlags.secondPassed = 1;
    incrementTime_reversible(&time, &timeFlags, 0);
}
*/


//Diskret: Alle 128 Interrupts 3ms entfernen. Alternativ, stetig: Bresenham Algorithmus (hier)
ISR(TIMER2_COMPA_vect){ //called every ms
    static uint16_t acc = 0;
    acc += 1000;

    if(acc >= 1024){
        acc -= 1024;
        tick_ms(&timeFlags, 0);
        debouncePass(1);
        incrementTime_reversible(&time, &timeFlags, 0);
    }
}