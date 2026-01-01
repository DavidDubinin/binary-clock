#include <avr/io.h>
#include <util/delay.h>

typedef struct Time{
  uint8_t hours;
  uint8_t minutes;
} Time;

uint16_t internal_time;
Time clock_time;


//[pd3:7,pc0:5] for time output
//takes an int and sets the correct bits for leds
void updateLEDs(){
  // 02:30 -> 00010 hours and 011110 minutes
  // 76543210 76543210
  // 00000000 00000000

  // reverse binary? idk wha tto do here


  PORTD = (clock_time.hours << 5);
  //PORTD &= ~(t_ptr->hours);

  PORTC = (clock_time.hours);
  //PORTC &= ~(t_ptr->minutes << 2);
}
//^^^ IMPROTANT!!! THIS IS BAD AND NEEDS TO BE FIXED

int convertToTime(){
  if (internal_time > 1439){
    return 1;
  }
  else{
    clock_time.hours = internal_time / 60;
    clock_time.minutes = internal_time % 60;
    return 0;
  }
}

int setTime(uint8_t hours, uint8_t minutes){
  if(hours <= 24 && minutes <= 59){
    internal_time = hours * 60 + minutes; 
    return 0;
  }
  else{
    return 1;
  }
}

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

  setTime(17, 42);
  while(1) {
    if(convertToTime() != 0){
      updateLEDs();
    };
    internal_time += 1;
    _delay_ms(1000.0);
  }
  
  return 0;
}



//pc0:5 - 0c1b, rest 0c1a