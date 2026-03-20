#include "uart.h"

#include "time.h"
#include <avr/io.h>
#include <string.h>

// ceil(log10(2^n))=5 <- anzahl an dezimalstellen bei n bit 

#define MAX_DIGITS 5
#define NUMSTRING_SIZE MAX_DIGITS+1

static inline char digitToASCII(uint8_t digit){
    return '0'+ (char)digit;
}

static void extractDigits(uint16_t num, uint8_t* buffer){
    buffer[0] = num / 10000;
    buffer[1] = (num / 1000) % 10;
    buffer[2] = (num / 100) % 10;
    buffer[3] = (num / 10) % 10;
    buffer[4] = num % 10;
}


static void numToString(uint16_t num, char* buffer){ 
    uint8_t digits[MAX_DIGITS] = {0,0,0,0,0};

    extractDigits(num, digits);

    for(uint8_t i = 0; i < MAX_DIGITS; i++){
        buffer[i] = digitToASCII(digits[i]);
    }
    
    buffer[MAX_DIGITS] = '\0';
}

static void txUart(char data){
    while(!(UCSR0A & (1 << UDRE0)));//UDREn: USART Data Register Empty
    UDR0 = data;
}

static uint8_t rxUart(void) {
    if((UCSR0A & (1 << RXC0))){
        uint8_t data = UDR0;
        return data;
    }
    return 0x00;
}

static void receiveString(char* buffer, uint8_t length){
    for(uint8_t i = 0; i < length; i++){
        buffer[i] = rxUart();
    }
}

static void sendString(char* string, uint8_t length){
    for(uint8_t i = 0; i < length; i++){
        txUart(string[i]);
    }
}

void initUart(void){
    UCSR0A |= (1 << U2X0);//Double Speed
    UCSR0B |= (1 << TXEN0) | (1 << RXEN0);//Enables TX and RX
    //UCSR0B |= (1 << RXCIE0); //RX Complete Interrupt Enable

    UCSR0C &= ~(1 << UMSEL00) & ~(1 << UMSEL01); // Asynchronous USART
    UCSR0C &= ~(1 << UPM00) & ~(1 << UPM01); //no parity
    UCSR0C &= ~(1 << USBS0); //1stop bit

    UCSR0B &= ~(1 << UCSZ02);
    UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01); // 8 data bits

    UBRR0H = (uint8_t)(BAUDRATE >> 8);
    UBRR0L = (uint8_t)(BAUDRATE);
}

static void sendTime(void) {
    Time t = getTime();

    char stringBuffer[NUMSTRING_SIZE] = {0,0,0,0,0,0};

    sendString("willkommen im hardwarelabor! viel spaß!\r\n", 43);
    numToString(t.hours, stringBuffer);
    sendString(stringBuffer, NUMSTRING_SIZE);
    sendString(":", 1);
    numToString(t.minutes, stringBuffer);
    sendString(stringBuffer, NUMSTRING_SIZE);
    sendString(":", 1);
    numToString(t.seconds, stringBuffer);
    sendString(stringBuffer, NUMSTRING_SIZE);
    sendString(":", 1);
    numToString(t.milliseconds, stringBuffer);
    sendString(stringBuffer, NUMSTRING_SIZE);

    sendString("\r\n", 3);

}

void processUart(void){
    char commandBuffer[4] = {0};
    receiveString(commandBuffer, 4);

    if(strcmp(commandBuffer, UART_RX_SYM_TIME_REQ) == 0){
        sendTime();
    }
}