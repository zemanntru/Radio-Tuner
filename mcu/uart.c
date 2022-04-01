#include "libraries/uart.h"

FILE uart_output;
FILE uart_input;

void usart_init()
{
    /* Set baud rate */
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    /* Enable receiver and transmitter */
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);
    /* Set frame format: 8data, 2stop bit */
    UCSR0C = (1<<USBS0) | (3<<UCSZ00);

    uart_output.put = (void*)usart_putchar;
    uart_output.get = NULL;
    uart_output.flags = _FDEV_SETUP_WRITE;
    uart_output.udata = 0;

    uart_input.put = NULL;
    uart_input.get = (void*)usart_getchar;
    uart_input.flags = _FDEV_SETUP_READ;
    uart_input.udata = 0;

    /* Tie stdin and stdout to USART */
    stdout = &uart_output;
    stdin  = &uart_input;
}

void usart_send( unsigned char data )
{
    /* Wait for empty transmit buffer */
    while ( !( UCSR0A & (1<<UDRE0)) );
    /* Put data into buffer, sends the data */
    UDR0 = data;
}

unsigned char usart_recv( void )
{
    /* Wait for data to be received */
    while ( !(UCSR0A & (1<<RXC0)) )
    ;
    /* Get and return received data from buffer */
    return UDR0;
}

void usart_putchar(unsigned char c, FILE *stream)
{
    if(c == 10) {           // \n ascii is 10
        usart_send(10);     // \r ascii is 13
        usart_send(13);
    } else
        usart_send(c);
}

unsigned char usart_getchar(FILE *stream) 
{
    return (unsigned char) usart_recv();
}