#ifndef BUTTONS_H
#define BUTTONS_H

#include <avr/io.h>

#define DEBOUNCE_TIME 150 //in ms
#define BUTTON_COUNT 3

typedef struct {
    uint8_t dispPressed;
    uint8_t setPressed;
    uint8_t dbgPressed;
} ButtonFlags;

extern volatile ButtonFlags buttonFlags;

void debouncePass_ms(void);

#endif