#ifndef SETUP_MODE_H
#define SETUP_MODE_H


typedef enum {
  OFF,
  HOURS,
  MINUTES
} SetupMode;

extern volatile SetupMode setupMode;


#endif