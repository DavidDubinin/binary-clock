#include "time.h"

#include <avr/interrupt.h>

volatile Time time;
volatile uint8_t minutesMode = 1;

volatile uint8_t secondPassed = 0;
volatile uint8_t minutePassed = 0;
volatile uint8_t hourPassed = 0;


//time = 86 398 -> 86 399 ->  86 400 mod 86 400 => 0
static void incrementTime(void){
    time = (time + 1) % 86400;

    //update disp jede Minute wenn in Minutenmodus oder jede Sekunde wenn nicht
    if(!minutesMode || time % 60 == 0){
    setLeds(calcTime());
    }
}

ISR(TIMER2_COMPA_vect){
    incrementTime();
}