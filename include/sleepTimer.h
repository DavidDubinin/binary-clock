#ifndef SLEEPTIMER_H
#define SLEEPTIMER_H

#include <avr/io.h>


#define SLEEP_TIME (uint32_t)5*1000 //in ms

extern volatile uint32_t sleepTimer;

void trySleep(void);

void sleepTimerPass_ms(void);

#endif