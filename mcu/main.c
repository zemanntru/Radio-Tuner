
#include "libraries/misc.h"

int main(void) 
{
    /* initialize all necessary libraries */
    usart_init();
    twi_init();
    si5351_init();
    misc_init();
    /*
    setup_PLL(SI5351_PLL_A, 36, 0, 1);
    clock_gen(SI5351_PLL_A, SI5351_PORT0, 8, 0, 1);
    clock_gen(SI5351_PLL_A, SI5351_PORT1, 8, 0, 1);
    set_phase(SI5351_PORT1, 36);
    enable_clocks(true);*/

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
        //_delay_ms(1000);
    }
    return 0;
}

