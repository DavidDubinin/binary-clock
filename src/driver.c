#include "driver.h"
#include <avr/interrupt.h>

#include <util/atomic.h>


//PB0 und PB5 als active-high input
void initUnused(void){
    cli();
    DDRB  &= ~(1 << PB0) & ~(1 << PB5);  
    PORTB |= (1 << PB0) | ( 1<< PB5);
    sei();
}

void initLeds(void) {
    cli();
    DDRD |= (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7); // Stundenpins, 5 bit
    DDRC |= (1 << PC0) | (1 << PC1) | (1 << PC2) | (1 << PC3) | (1 << PC4) | (1 << PC5); // Minutenpins, 6 bit
    sei();
}

void initButtons(void) {
    cli();
    //set Button pins as inputs
    DDRD &= ~(1 << PD2);
    DDRB &= ~(1 << PB3) & ~(1 << PB4);
    //enable internal pullups for Buttons
    PORTD |= (1 << PD2);
    PORTB |= (1 << PB3) | (1 << PB4);

    //set INT0 trigger to falling edge
    EICRA |= (1 << ISC01);
    EICRA &= ~(1 << ISC00);
    //Enable INT0
    EIMSK |= (1 << INT0);

    //Enable PCINT7..0 interrupt und Maske for PCINT3..4
    PCICR |= (1 << PCIE0);
    PCMSK0 |= (1 << PCINT3) | (1 << PCINT4);
    sei();
}
 
void initPwm(void) {
    cli();
    DDRB |= (1 << DDB1) | (1 << DDB2); // OC1A/PB1 bzw OC1B/PB2 auf Output für Stunden/Minuten

    TCCR1A |= (1 << WGM10) | (1 << WGM11) | (1 << COM1A1) 
            | (1 << COM1B1) | (1 << COM1A0) | (1 << COM1B0);
            
    TCCR1B |= (1 << WGM12) | (1 << CS10);
    TCCR1B &= ~(1 << WGM13) & ~(1 << CS11) & ~(1 << CS12);

    OCR1A = (1023 * DUTY_CYCLE) / 100;
    OCR1B = (1023 * DUTY_CYCLE) / 100;
    sei();
    }

//Watch Crystal 32 768 Hz, Prescaler 128, F_OCnx = 1/2 Hz (1 interrupt per sec) => OCRnx = 255  
//Toggle OC2A on Compare Match, CTC Mode, TOP=OCR2A
void initQuartz_seconds(void){
    cli();
    //1. Disable the Timer/Counter2 interrupts by clearing OCIE2x and TOIE2
    TIMSK2 &= ~(1<<OCIE2A) & ~(1<<OCIE2B) & ~(1<<TOIE2);

    //2. Select clock source by setting AS2 as appropriate
    //Enable Asynchronous Timer/Counter2
    ASSR |= (1 << AS2);

    //3. Write new values to TCNT2, OCR2x, and TCCR2x.
    //Toggle OC2A on Compare Match

    TCNT2 = 0x00; //reset timer
    TCCR2A |= (1 << COM2A0);
    TCCR2A &= ~(1 << COM2A1);

    //CTC-Mode
    TCCR2A |= (1 << WGM21);
    TCCR2A &= ~(1 << WGM20);
    TCCR2B &= ~(1 << WGM22);

    //CS selection mit Prescaler = 128
    TCCR2B |= (1 << CS20) | (1 << CS22);
    TCCR2B &= ~(1 << CS21);

    //Compare Match bei Timer = 255 => OC2A Interrupt Flag Set
    OCR2A = 0xFF;

    //4. To switch to asynchronous operation: Wait for TCN2xUB, OCR2xUB, and TCR2xUB
    //ASSR = xxx1 1111 BAD
    //ASSR = xxx0 0000
    while(ASSR & ((1<<TCN2UB) | (1<<OCR2AUB) | (1<<OCR2BUB) | (1<<TCR2AUB) | (1<<TCR2BUB))){
    __builtin_avr_nop(); // sigma approved!!!
    }

    //5. Clear the Timer/Counter2 Interrupt Flags
    TIFR2 |= (1 << OCF2B) | (1 << OCF2A) | (1 << TOV2);

    //6. Enable interrupts, if needed
    //Enable Output Compare Match Interrupt A
    TIMSK2 |= (1 << OCIE2A);
    sei();
}


//Watch Crystal 32 768 Hz, Prescaler 1, F_OCnx = 512Hz (1 interrupt per ms) => OCRnx = 31  
//Toggle OC2A on Compare Match, CTC Mode, TOP=OCR2A

// F_OCnx = F_clkIO / (2 * N * (1 + OCRnx))
// F_Interrupt = 2 * F_OCnx (2x Interrupts in einer Periode CTC)
// F_interrupt = 2 * ( F_clkIO / (2 * N * (1 + OCRnx)) )
// F_interrupt = 2 * ( 32 768Hz / (2 * 1 * (1 + OCRnx)) )
// F_interrupt = 2 * ( 32 768Hz / (2 * 1 * (1 + 63)) ) = 512Hz F_Interrupts ↯ wollen aber 1024Hz 


// Gleichung
// 1024 Hz = 32 768Hz / (1*(1 + OCRnx))          | * 1*(1+OCRnx)
// 1024 Hz * (1 + OCRnx) = 32 768 Hz             | / 1024
// 1 + OCRnx = 32 768 Hz / 1024 Hz
// 1 + OCRnx = 32                                | - 1
// OCRnx = 31 bei 1024 Iterrupts/s

void initQuartz_ms(void){
    cli();
    //1. Disable the Timer/Counter2 interrupts by clearing OCIE2x and TOIE2
    TIMSK2 &= ~(1<<OCIE2A) & ~(1<<OCIE2B) & ~(1<<TOIE2);

    //2. Select clock source by setting AS2 as appropriate
    //Enable Asynchronous Timer/Counter2
    ASSR |= (1 << AS2);

    //3. Write new values to TCNT2, OCR2x, and TCCR2x.
    //Toggle OC2A on Compare Match

    TCNT2 = 0x00; //reset timer
    TCCR2A |= (1 << COM2A0);
    TCCR2A &= ~(1 << COM2A1);

    //CTC-Mode
    TCCR2A |= (1 << WGM21);
    TCCR2A &= ~(1 << WGM20);
    TCCR2B &= ~(1 << WGM22);

    //CS selection mit Prescaler = 1 (= No prescaler)
    TCCR2B |= (1 << CS20);
    TCCR2B &= ~(1 << CS22) & ~(1 << CS21);

    //Compare Match bei Timer = 31 => OC2A Interrupt Flag Set | siehe Rechnung oben
    OCR2A = 31;

    //4. To switch to asynchronous operation: Wait for TCN2xUB, OCR2xUB, and TCR2xUB
    //ASSR = xxx1 1111 BAD
    //ASSR = xxx0 0000
    while(ASSR & ((1<<TCN2UB) | (1<<OCR2AUB) | (1<<OCR2BUB) | (1<<TCR2AUB) | (1<<TCR2BUB))){
    __builtin_avr_nop(); // sigma approved!!!
    }

    //5. Clear the Timer/Counter2 Interrupt Flags
    TIFR2 |= (1 << OCF2B) | (1 << OCF2A) | (1 << TOV2);

    //6. Enable interrupts, if needed
    //Enable Output Compare Match Interrupt A
    TIMSK2 |= (1 << OCIE2A);
    sei();
}

void setLeds(uint8_t hourLedsVal, uint8_t minuteLedsVal) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        PORTD = __builtin_avr_insert_bits(0x01234FFF, hourLedsVal, PORTD); // portd 7 bis 3  stunden
        PORTC = __builtin_avr_insert_bits(0xFF012345, minuteLedsVal, PORTC); //portc 5 bis 0 Sekunden
    }
}