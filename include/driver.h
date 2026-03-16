#ifndef DRIVER_H
#define DRIVER_H


#include <avr/io.h>

#define DUTY_CYCLE 1 // in %

/*
Sets relevant DDRx registers
*/
void initLeds(void);

/*
sets up Button DDRx registers, internal pullups and configures INT0 and PCINT0 interrupts
*/
void initButtons(void);

/*
 Set OC1A/OC1B on Compare Match, clear OC1A/OC1B at TOP 
 Fast PWM, 10-bit, TOP = 0x03FF, Update of OCR1x at TOP, TOV1 Flag set on TOP
 CS: clkI/O/1 (No prescaling) 
 */
void initPwm(void);

/*
Watch Crystal 32 768 Hz, Prescaler 128, F_OCnx = 1/2 Hz (1 interrupt per sec) => OCRnx = 255  
Toggle OC2A on Compare Match, CTC Mode, TOP=OCR2A 
*/
void initQuartz(void);

/*
correctly assigns values to PORTD and PORTC, takes 5 bits and 6 bits respectively
*/
void setLeds(uint8_t hourLedsVal, uint8_t minuteLedsVal);
// Hier leuchten die LHS (Lichtaussendende Halbleiterbauelementeinheit mit elektroluminiszenter Strahlungsfunktion)


#endif