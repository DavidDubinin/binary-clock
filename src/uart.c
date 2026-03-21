#include "uart.h"

#include "time.h"
#include <avr/io.h>
#include <string.h>

// ceil(log10(2^n))=MAX_DIGITS <- max anzahl an dezimalstellen bei n bit 
#define MAX_DIGITS 5

static inline char digitToAscii(uint8_t digit){
    return '0'+ (char)digit;
}

static inline uint8_t asciiToDigit(char digit){
    return (uint8_t)digit - '0';
}

static void extractDigits(uint16_t num, uint8_t* buffer){
    buffer[0] = num / 10000;
    buffer[1] = (num / 1000) % 10;
    buffer[2] = (num / 100) % 10;
    buffer[3] = (num / 10) % 10;
    buffer[4] = num % 10;
}

static uint8_t findFirstIndex(uint8_t* digits, uint8_t digitCount){
    uint8_t i = 0;
    while(digits[i] == 0 && i < MAX_DIGITS-digitCount){
        i++;
    }
    return i;
}

static void numToString(uint16_t num, char* buffer, uint8_t size){ 
    uint8_t digits[MAX_DIGITS] = {0};
    extractDigits(num, digits);

    uint8_t start = findFirstIndex(digits, size-1);

    for(uint8_t i = 0; i < size-1; i++){
        buffer[i] = digitToAscii(digits[start+i]);
    }
    buffer[size-1] = '\0';
}

static void txUart(char data){
    while(!(UCSR0A & (1 << UDRE0)));//UDREn: USART Data Register Empty
    UDR0 = data;
}

static char rxUart(void) {
    if((UCSR0A & (1 << RXC0))){
        uint8_t data = UDR0;
        return (char)data;
    }
    return 0x00;
}

static void receiveString(char* buffer, uint8_t length){
    for(uint8_t i = 0; i < length; i++){
        buffer[i] = rxUart();
    }
}

//doesnt send last symbol (\0)
static void sendString(char* string, uint8_t length){
    for(uint8_t i = 0; i < length-1; i++){
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

    char hourString[3] = {0};
    char minString[3] = {0};
    char secString[3] = {0};
    char msString[4] = {0};

    numToString(t.hours, hourString, sizeof(hourString));
    sendString(hourString, sizeof(hourString));
    sendString(":", 2);

    numToString(t.minutes, minString, sizeof(minString));
    sendString(minString, sizeof(minString));
    sendString(":", 2);

    numToString(t.seconds, secString, sizeof(secString));
    sendString(secString, sizeof(secString));
    sendString(".", 2);

    numToString(t.milliseconds, msString, sizeof(msString));
    sendString(msString, sizeof(msString));
    sendString("\r\n", 3);

}

//WIP
static void receiveTime(void){
    char receivedData[sizeof(Time)] = {0};
    receiveString(receivedData, sizeof(receivedData));

    Time received;
    if(setTime(&received) == -1){

    }

}

void processUart(void){
    /*
    char commandBuffer[COMMAND_LENGTH] = {0};
    receiveString(commandBuffer, COMMAND_LENGTH);

    if(strncmp(commandBuffer, UART_RX_SYM_TIME_REQ, COMMAND_LENGTH) == 0){
        sendTime();
    }
    */

    switch (rxUart())
    {
    case UART_RX_SYM_TIME_REQ:
        sendTime();
        break;
    case UART_RX_SYM_TIME_PROVIDE:
        receiveTime();
        break;
    default:
        break;
    }

}