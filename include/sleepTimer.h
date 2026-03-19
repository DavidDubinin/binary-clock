#ifndef SLEEPTIMER_H
#define SLEEPTIMER_H

#include <avr/io.h>


#define SLEEP_TIME (uint32_t)30*1000 //in ms

volatile uint32_t sleepTimer;

/*
Sets unused GPIO pins to Output LOW
*/
void configMinimalPower(void);

void trySleep(void);

void sleepTimerPass_ms(void);

#endif