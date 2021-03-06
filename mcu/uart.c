#include "libraries/uart.h"

FILE uart_output;
FILE uart_input;
volatile char msg_buffer[128];
volatile int msg_back;
int msg_front;

void usart_init()
{
    /* Set baud rate */
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    /* Enable receiver and transmitter */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);
    /* Set frame format: 8data, 2stop bit */
    UCSR0C = _BV(USBS0) | _BV(UCSZ01) | _BV(UCSZ00);

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

    msg_front = 0;
    msg_back = 0;
    sei();
}

void usart_send(byte data)
{
    /* Wait for empty transmit buffer */
    while ( !( UCSR0A & _BV(UDRE0)) );
    /* Put data into buffer, sends the data */
    UDR0 = data;
}

byte usart_recv()
{
    /* Wait for data to be received */
    while ( !(UCSR0A & _BV(RXC0)) );
    /* Get and return received data from buffer */
    return UDR0;
}

void usart_putchar(byte c, FILE *stream)
{
    if(c == 10) {           // \n ascii is 10
        usart_send(10);     // \r ascii is 13
        usart_send(13);
    } else
        usart_send(c);
}

byte usart_getchar(FILE *stream) 
{
    return (byte) usart_recv();
}

ISR(USART0_RX_vect)
{
    /* setup interrupt handler, reads the data register and exits */
    msg_buffer[msg_back++] = UDR0;
    msg_back = msg_back & 0x7F;
}