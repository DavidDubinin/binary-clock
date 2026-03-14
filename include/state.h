#ifndef STATE_H
#define STATE_H


typedef enum {
  INIT,
  RUNNING,
  DISP,
  SET,
  DBG
} State;

extern volatile State state;


#endif