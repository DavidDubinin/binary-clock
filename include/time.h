#ifndef TIME_H
#define TIME_H


#include <avr/io.h>


typedef struct {
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
} Time;

extern volatile Time time;
extern volatile uint8_t minutesMode;

extern volatile uint8_t secondPassed;
extern volatile uint8_t minutePassed;
extern volatile uint8_t hourPassed;

#endif