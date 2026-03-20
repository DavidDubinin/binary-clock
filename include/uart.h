#ifndef UART_H
#define UART_H
#include <stdint.h>

#define BAUDRATE (uint16_t)12 //9600 Baud, aus Datasheet für 2xSpeed
#define UART_RX_SYM_TIME_REQ "GET"
#define UART_RX_SYM_TIME_PROVIDE "SET"

//initialisiert 8N1 uart  
void initUart(void);
void processUart(void);

#endif