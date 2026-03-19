#include "sleepTimer.h"
#include <avr/sleep.h>
#include <util/atomic.h>

volatile uint32_t sleepTimer = SLEEP_TIME;

void trySleep(void){
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        if(sleepTimer == 0) sleep_cpu();
    }
}

void sleepTimerPass_ms(void){
    if(sleepTimer != 0) sleepTimer--;
}