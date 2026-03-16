#include "time.h"
#include "driver.h"
#include "state.h"

#include <avr/interrupt.h>

volatile Time time;
volatile TimeFlags timeFlags;

static void updateSeconds(){
    if (time.seconds == 59){
        time.seconds = 0;
        timeFlags.minutePassed = 1;
    }
    else{
        time.seconds++;
    }  
}

static void updateMinutes(){
    if (time.minutes == 59){
        time.minutes = 0;
        timeFlags.hourPassed = 1;
    }
    else{
        time.minutes++;
    }
}

static void updateHours(){
    if (time.hours == 23){
        time.hours = 0;
    }
    else{
        time.hours++;
    }
}

static void incrementTime(void){ 
    if(timeFlags.secondPassed){
        timeFlags.secondPassed = 0;
        updateSeconds();
        if(state == SHOW_SECONDS) setLeds(time.hours,time.seconds);
    }
    if(timeFlags.minutePassed){
        timeFlags.minutePassed = 0;
        updateMinutes();
        if(state == SHOW_MINUTES) setLeds(time.hours, time.minutes);
    }
    if(timeFlags.hourPassed){
        timeFlags.hourPassed = 0;
        updateHours();
    }
}

ISR(TIMER2_COMPA_vect){ //called every second
    timeFlags.secondPassed = 1;
    incrementTime();
}