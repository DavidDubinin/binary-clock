#include "time.h"

#include "driver.h"
#include "state.h"
#include <avr/interrupt.h>

volatile static Time time;
volatile static TimeFlags timeFlags;

static inline uint8_t timeIsValid(Time t){
    return t.seconds < 60 && t.minutes < 60 && t.hours < 24;
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
    else{
        if (t->minutes == 0){
            t->minutes = 59;
            tflags->hourPassed = 1;
    }
        else{
            t->minutes--;
        }
    }
}

static void updateHours(Time* t, TimeFlags* tflags, uint8_t backwards){
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

static void incrementTimeStruct_reversible(Time* t, TimeFlags* tflags, uint8_t backwards){
    if(tflags->secondPassed){
        tflags->secondPassed = 0;
        updateSeconds(t, tflags, backwards);
        if(state == SHOW_SECONDS) setLeds(t->hours,t->seconds);
    }
    if(tflags->minutePassed){
        tflags->minutePassed = 0;
        updateMinutes(t, tflags, backwards);
        if(state == SHOW_MINUTES) setLeds(t->hours,t->minutes);
    }
    if(tflags->hourPassed){
        tflags->hourPassed = 0;
        updateHours(t, tflags, backwards);
    }
}

void incrementTimeStruct(Time* t, TimeFlags* tflags){ 
    incrementTimeStruct_reversible(t,tflags,0);
}

void decrementTimeStruct(Time* t, TimeFlags* tflags) {
    incrementTimeStruct_reversible(t,tflags,1);
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

ISR(TIMER2_COMPA_vect){ //called every second
    timeFlags.secondPassed = 1;
    incrementTimeStruct(&time, &timeFlags);
}