#ifndef STATE_H
#define STATE_H

#include <avr/io.h>

/*State Machine*/
typedef enum {
  INIT,
  SHOW_MINUTES,
  SHOW_SECONDS,
  SETUP_HOURS,
  SETUP_MINUTES,
  DBG
} State;

extern volatile State state;

/*----------------
STATE CHANGE LOGIC
----------------*/
/*
void initialized(void); //INIT -> SHOW_MINUTES
void toggleMinuteMode(void); //SHOW_MINUTES <-> SHOW_SECONDS
void toggleDbg(void); //SHOW_X <-> DBG (always goes back to SHOW_MINUTES)
void enterSetupMode(void); //SHOW_X -> SETUP_HOURS
void continueSetup(void); //SETUP_HOURS -> SETUP_MINUTES
void finishSetup(void); //SETUP_MINUTES -> SHOW_MINUTES
void count(uint8_t up); //SETUP_X -> SETUP_X
*/

/*------------
STATE LOGIC
------------*/

void initState(void); //INIT
void showMinutesState(void); //SHOW_MINUTES
void showSecondsState(void); //SHOW_SECONDS
void setupHoursState(void); //SETUP_HOURS
void setupMinutesState(void); //SETUP_MINUTES
void debugState(void); //DBG

#endif