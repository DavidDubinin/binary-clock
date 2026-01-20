#include <avr/io.h>
#include <util/delay.h>

typedef struct Time{
  uint8_t hours;
  uint8_t minutes;
} Time;

uint16_t internal_time;
Time clock_time;


int main(void)
{

  DDRD |= (1 << PD3) |  (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7);
  DDRC |= (1 << PC0) |  (1 << PC1) | (1 << PC2) | (1 << PC3) | (1 << PC4) | (1 << PC5);

  // Set OC1A (PB1) and OC1B (PB2) as outputs
  DDRB |= (1 << PB1) | (1 << PB2);

  // Set Fast PWM 8-bit mode (WGM10 = 1, WGM11 = 0, WGM12 = 1, WGM13 = 0)
  TCCR1A = (1 << WGM10);                 // WGM11=0, WGM10=1
  TCCR1B = (1 << WGM12);                 // WGM13=0, WGM12=1

  // Set non-inverting mode for both channels (COM1A1=1, COM1B1=1)
  TCCR1A |= (1 << COM1A1) | (1 << COM1B1);

  // Set prescaler to 64 (CS11=1, CS10=1)
  TCCR1B |= (1 << CS11) | (1 << CS10);

  // Set duty cycle to 99% (OCR1A/OCR1B = 99% of 255 ~ 252)
  OCR1A = 254;
  OCR1B = 254;

  PORTD |= (1 << PD3) |  (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7);
  PORTC |= (1 << PC0) |  (1 << PC1) | (1 << PC2) | (1 << PC3) | (1 << PC4) | (1 << PC5);

  
  return 0;
}



//pc0:5 - 0c1b, rest 0c1a