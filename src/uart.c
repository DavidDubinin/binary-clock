#include "uart.h"

#include "time.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#define UBRR_VALUE ((F_CPU)/(8*BAUDRATE)-1) //double speed

void initUart(void){
    UCSR0A |= (1 << U2X0);//Double Speed
    UCSR0B |= (1 << TXEN0) | (1 << RXEN0);//Enables TX and RX
    //UCSR0B |= (1 << RXCIE0); //RX Complete Interrupt Enable

    UCSR0C &= ~(1 << UMSEL00) & ~(1 << UMSEL01); // Asynchronous USART
    UCSR0C &= ~(1 << UPM00) & ~(1 << UPM01); //no parity
    UCSR0C &= ~(1 << USBS0); //1stop bit

    UCSR0B &= ~(1 << UCSZ02);
    UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01); // 8 data bits

    UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
    UBRR0L = (uint8_t)(UBRR_VALUE);
}


static uint8_t rxUart(void) {
    if((UCSR0A & (1 << RXC0))){
        uint8_t data = UDR0;
        return data;
    }
    return 0x00;
}

static void txUart(uint8_t data){
    while(!(UCSR0A & (1 << UDRE0)));//UDREn: USART Data Register Empty
    UDR0 = data;
}

static void sendTime(void) {
    Time t = getTime();
    uint8_t millisecondsH = (uint8_t)(t.milliseconds >> 8);
    uint8_t millisecondsL = (uint8_t)(t.milliseconds);

    txUart(t.hours);
    txUart(t.minutes);
    txUart(t.seconds);
    txUart(millisecondsH);
    txUart(millisecondsL);
    txUart(UART_STOP_SYM);
}

void processUart(void){
    switch(rxUart()){
        case UART_RX_SYM_TIME_REQ:
            sendTime();
            break;
        default:
            break;
    }
}