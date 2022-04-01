#ifndef UART_HEADER
#define UART_HEADER

#include "pch.h"

void usart_init();
void usart_send(byte data);
byte usart_recv();
void usart_putchar(byte c, FILE *stream);
byte usart_getchar(FILE *stream);

#endif