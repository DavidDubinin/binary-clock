#ifndef STATE_H
#define STATE_H

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

void initialized(void); //INIT -> SHOW_MINUTES
void toggleMinuteMode(void); //SHOW_MINUTES <-> SHOW_SECONDS
void toggleDbg(void); //SHOW_MINUTES or SHOW_SECONDS <-> DBG
void enterSetupMode(void); //SHOW_MINUTES or SHOW_SECONDS -> SETUP_HOURS
void continueSetup(void); //SETUP_HOURS -> SETUP_MINUTES
void finishSetup(void); //SETUP_MINUTES -> SHOW_MINUTES

void initState(void);
void showMinutesState(void);
void showSecondsState(void);
void setupHoursState(void);
void setupMinutesState(void);
void debugState(void);

#endif