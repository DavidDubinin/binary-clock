#ifndef TIME_H
#define TIME_H


#include <avr/io.h>


typedef struct {
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;  
} Time;

typedef struct {
  uint8_t hourPassed;
  uint8_t minutePassed;
  uint8_t secondPassed;
} TimeFlags;

int setTime(Time* newTime);
Time getTime();

void incrementTimeStruct(Time* t, TimeFlags* tflags);
void decrementTimeStruct(Time* t, TimeFlags* tflags);

#endif