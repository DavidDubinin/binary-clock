#ifndef UART_H
#define UART_H
#include <stdint.h>

#define BAUDRATE (uint16_t)9600
#define UART_RX_SYM_TIME_REQ (uint8_t) 0xAA
#define UART_STOP_SYM (uint8_t) '\r'

//initialisiert 8N1 uart  
void initUart(void);
void processUart(void);

#endif