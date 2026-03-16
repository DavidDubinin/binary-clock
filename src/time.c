#include "time.h"

#include <avr/interrupt.h>

volatile Time time;
volatile TimeFlags timeFlags;
volatile uint8_t minutesMode = 1;

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
        if(!minutesMode) setLeds(time.hours,time.seconds);
    }
    if(timeFlags.minutePassed){
        timeFlags.minutePassed = 0;
        updateMinutes();
        if(minutesMode) setLeds(time.hours, time.minutes);
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