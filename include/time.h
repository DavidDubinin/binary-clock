#ifndef TIME_H
#define TIME_H


#include <avr/io.h>

typedef struct {
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds; 
  uint16_t milliseconds;
} Time;

typedef struct {
  uint8_t hourPassed;
  uint8_t minutePassed;
  uint8_t secondPassed;
} TimeFlags;

extern volatile TimeFlags timeFlags_external;

int setTime(Time* newTime);
Time getTime(void);

void incrementTimeSetup_reversible(Time* newTime, TimeFlags* newTflags, uint8_t backwards);

#endif