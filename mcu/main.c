
#include "libraries/misc.h"

int main(void) 
{
    /* initialize all necessary libraries */
    usart_init();
    twi_init();
    si5351_init();
    misc_init();
    
    while(1)
    {
        read_encoder();

        // detect RX/TX switch and activate the relay
        bool flag = (PINA & _BV(PINA1)) >> PINA1;
        if(flag) {
            PORTD |= (_BV(PORTD5) | _BV(PORTD4));
        } else {
            PORTD &= ~(_BV(PORTD5) | _BV(PORTD4));
        }
        _delay_ms(1000);
    }
    return 0;
}

