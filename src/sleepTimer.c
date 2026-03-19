#include "sleepTimer.h"
#include <avr/sleep.h>
#include <util/atomic.h>

volatile uint32_t sleepTimer = SLEEP_TIME;

void trySleep(void){
    ATOMIC_BLOCK(ATOMIC_FORCEON){
        if(sleepTimer == 0) sleep_enable();
    }
    sleep_cpu();
    sleep_disable();
}

void sleepTimerPass_ms(void){
    if(sleepTimer != 0) sleepTimer--;
}