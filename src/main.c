#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main(void) {
  DDRC |= (1<<PC0);
  PORTC |= (1<<PC0);
  while(1){asm("nop");}
}

//[pd3:7,pc0:5] for time output
//pc0:5 - 0c1b, rest 0c1a