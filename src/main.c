#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main(void) {
  DDRC |= (1<<PC0);
  PORTC |= (1<<PC0);
  while(1){asm("nop");}
}

//[pc5:0,pd7:3] for time output
//pc5:0 - 0c1a, rest 0c1b