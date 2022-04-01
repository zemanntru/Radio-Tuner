#ifndef UART_HEADER
#define UART_HEADER

#include "pch.h"

void usart_init();
void usart_send(unsigned char data);
unsigned char usart_recv( void );
void usart_putchar(unsigned char c, FILE *stream);
unsigned char usart_getchar(FILE *stream);

#endif