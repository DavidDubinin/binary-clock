#include "uart.h"

#include "time.h"
#include <avr/io.h>

// ceil(log10(2^n))=MAX_DIGITS <- max anzahl an dezimalstellen bei n bit 
#define MAX_DIGITS 5

static inline char digitToAscii(uint8_t digit){
    return '0'+ (char)digit;
}

static inline uint8_t asciiToDigit(char digit){
    return (uint8_t)digit - '0';
}

static inline uint16_t powOfTen(uint8_t significance){
    switch(significance){
        case 0: return 1;
        case 1: return 10;
        case 2: return 100;
        case 3: return 1000;
        case 4: return 10000;
    }
    return 0;
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
    while(i < MAX_DIGITS-digitCount && digits[i] == 0){
        i++;
    }
    return i;
}

//returns with nullterminator, expects nullterm
static void numToString(uint16_t num, char* buffer, uint8_t size){ 
    uint8_t digits[MAX_DIGITS] = {0};
    extractDigits(num, digits);

    uint8_t start = findFirstIndex(digits, size-1);

    for(uint8_t i = 0; i < size-1; i++){
        buffer[i] = digitToAscii(digits[start+i]);
    }
    buffer[size-1] = '\0';
}

static uint16_t dataToNum(char* buffer, uint8_t size){
    uint8_t significance = 0;
    uint16_t number = 0;
    for(int16_t i = size-1; i >= 0; i--){
        uint8_t val = asciiToDigit(buffer[i]);
        number += powOfTen(significance) * val;
        significance++;
    }
    return number;
}

static void txUart(char data){
    while(!(UCSR0A & (1 << UDRE0)));//UDREn: USART Data Register Empty
    UDR0 = data;
}

static uint8_t rxUart(char* data) {
    if((UCSR0A & (1 << RXC0))){
        *data = (char)UDR0;
        return 1;
    }
    return 0;
}

//assumes no null termination
static void receiveData(char* buffer, uint8_t length){
    for(uint8_t i = 0; i < length; i++){//for each character
        char received;
        while(!rxUart(&received));
        buffer[i] = received;
    }
}

//assumes string with null terminator
static void sendString(char* string, uint8_t length){
    for(uint8_t i = 0; i < length; i++){
        txUart(string[i]);
    }
}

//expects size with nullterm
static void sendNumber(uint16_t num, uint8_t size){
    char string[size];
    numToString(num, string, sizeof(string));
    sendString(string, sizeof(string));
}

static void sendTime(void) {
    Time t = getTime();

    sendString("Time: ", 7);
    sendNumber(t.hours, 3);
    sendString(":", 2);
    sendNumber(t.minutes, 3);
    sendString(":", 2);
    sendNumber(t.seconds, 3);
    sendString(".", 2);
    sendNumber(t.milliseconds, 4);
    sendString("\r\n", 3);
    
}
// 0123456789012
//S_HH_MM_SS_MSM (_ means any character)
static void receiveTime(void){
    char receivedData[9+4]; //9 for timedata, 4 for 1 space + 3 delimiters
    receiveData(receivedData, sizeof(receivedData));

    //assumes no null terminator
    char hourData[2] = { receivedData[1], receivedData[2] };
    char minData[2]  = { receivedData[4], receivedData[5] };
    char secData[2]  = { receivedData[7], receivedData[8] };
    char msData[3]   = { receivedData[10], receivedData[11], receivedData[12] };

    Time t;

    t.hours = (uint8_t)dataToNum(hourData, sizeof(hourData));
    t.minutes = (uint8_t)dataToNum(minData, sizeof(minData));
    t.seconds = (uint8_t)dataToNum(secData, sizeof(secData));
    t.milliseconds = (uint16_t)dataToNum(msData, sizeof(msData));

    if(setTime(&t)) sendString("Time set!\r\n", 12);
    else sendString("Time invalid!\r\n", 16);
}

static void invalidCommand(void){
    sendString("Sorry, that is not a valid command.\nValid commands are:\nG - to get the Time\nS hh:mm:ss.msm - to set the Time\r\n", 111);
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

void processUart(void){
    char command;
    if(rxUart(&command)) {
        switch (command){
        case UART_RX_SYM_TIME_REQ:
            sendTime();
            break;
        case UART_RX_SYM_TIME_PROVIDE:
            receiveTime();
            break;
        default:
            invalidCommand();
            break;
        }
    }


}