#ifndef STATE_H
#define STATE_H


typedef enum {
  INIT,
  RUNNING,
  SETUPMODE,
  DBG
} State;

extern volatile State state;


#endif