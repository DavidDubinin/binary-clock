#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>


#define DUTY_CYCLE 1 // in %
#define hour 15
#define minute 33
#define sec 10

volatile uint32_t time = (uint32_t)hour*60*60 + minute*60 + sec; // unit 1s //0 bis 24*60*60 = 86 400 (86 399 -> 0)

void setLeds(uint32_t time) { 
  uint8_t hours = time / (3600); // 000_0 1101 Stunden
  uint8_t minutes = (time % (3600)) / 60; // 00_00 1110 Minuten
  //^need to be reversed
  PORTD = __builtin_avr_insert_bits(0x01234FFF, hours, PORTD); // portd 7 bis 3  stunden
  PORTC = __builtin_avr_insert_bits(0xFF012345, minutes, PORTC); //portc 5 bis 0 minuten
} 

void incrementTime(void) { //time = 86 398 -> 86 399 ->  86 400 mod 86 400 => 0 
  time = (time + 1) % 86400;

  //update disp
  if(time % 60 == 0){
    setLeds(time);
  }
}

void initPwm(void) {
  // Set OC1A/OC1B on Compare Match, clear OC1A/OC1B at TOP 
  // ; Fast PWM, 10-bit, TOP = 0x03FF, Update of OCR1x at TOP, TOV1 Flag set on TOP
  // CS: clkI/O/1 (No prescaling) 
  TCCR1A |= (1 << WGM10) | (1 << WGM11) | (1 << COM1A1) 
         | (1 << COM1B1) | (1 << COM1A0) | (1 << COM1B0);
         
  TCCR1B |= (1 << WGM12) | (1 << CS10);
  TCCR1B &= ~(1 << WGM13) & ~(1 << CS11) & ~(1 << CS12);

  OCR1A = DUTY_CYCLE*1023/100;
  OCR1B = DUTY_CYCLE*1023/100;
  
}

void initQuartz(void){

}

void initPins(void) {
  DDRB |= (1 << DDB1) | (1 << DDB2); // OC1A/PB1 bzw OC1B/PB2 auf Output für Stunden/Minuten
  DDRD |= (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7); // Stundenpins, 5 bit
  DDRC |= (1 << PC0) | (1 << PC1) | (1 << PC2) | (1 << PC3) | (1 << PC4) | (1 << PC5); // Minutenpins, 6 bit
}

void testLeds(void){
  PORTD |= (1 << PORTD3) | (1 << PORTD4) | (1 << PORTD5) | (1 << PORTD6) | (1 << PORTD7); // Stunden
  PORTC |= (1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2) | (1 << PORTC3) | (1 << PORTC4) | (1 << PORTC5); // Minuten
}

void shittyTestLoop(void){
  setLeds(time);
  while(1){
    _delay_ms(1000);
    incrementTime();
  }
}

int main(void){
  initPins();
  initPwm();
  //testLeds();

  shittyTestLoop();

  return 0;
}
