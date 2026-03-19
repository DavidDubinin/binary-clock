#include "sleepTimer.h"

#include "driver.h"
#include "state.h"
#include "time.h"

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>


volatile uint32_t sleepTimer = SLEEP_TIME;

void configMinimalPower(void){
    set_sleep_mode(SLEEP_MODE_PWR_SAVE); // Prozessor Schläft, TIMER2 arbeitet weiter, ext Interrupt weckt es auf
    DDRB  &= ~(1 << PB0) & ~(1 << PB5);  //PB0 und PB5 als input
    PORTB |= (1 << PB0) | ( 1<< PB5); //pullups an

    power_twi_disable(); //reinit nötig wenn wieder enabled
    power_timer0_disable();
    power_adc_disable();
}


void trySleep(void){
    uint8_t sreg_local = SREG;
    cli();
    if(sleepTimer == 0){
        sleep_enable();
        disableLeds();
        sei();
        sleep_cpu();
        cli();
        sleep_disable();
    }
    SREG = sreg_local;//restores previous state
}

void sleepTimerPass_ms(void){
    if(sleepTimer != 0) sleepTimer--;
}