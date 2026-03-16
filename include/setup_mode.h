#ifndef SETUP_MODE_H
#define SETUP_MODE_H


#include <avr/io.h>

#define BLINK_TIMER 1000 //in ms (T)

typedef enum {
  OFF,
  HOURS,
  MINUTES
} SetupMode;

extern volatile SetupMode setupMode;
extern volatile uint16_t blinkTimer;



#endif