#ifndef BUTTONS_H
#define BUTTONS_H


#include <avr/io.h>

#define DEBOUNCE_TIME 100 //in ms
#define BUTTON_COUNT 3

extern volatile uint8_t debounce[BUTTON_COUNT];

void debouncePass(uint8_t ms);

#endif