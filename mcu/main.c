
#include "libraries/uart.h"
#include "libraries/encoder.h"
#include "libraries/si5351.h"

int main(void) 
{
    //CLKPR = 1<<CLKPCE;
    CLKPR = 0;
    /* initialize all necessary libraries */
    usart_init();
    twi_init();
    si5351_init();

    // set up PLL material
    setup_PLL(SI5351_PLL_A, 36, 0, 1);
    clock_gen(SI5351_PLL_A, SI5351_PORT0, 8, 0, 1);
    clock_gen(SI5351_PLL_A, SI5351_PORT1, 8, 0, 1);
    set_phase(SI5351_PORT1, 36);
    enable_clocks(1);
    
    DDRA &= (1<<DDA1);
    DDRD |= ((1<<DDD5) | (1<<DDD4));
    byte red, green, blue;

    srand(time(NULL));
    while(1)
    {
        // pll prompt
        printf("output frequency #1: %d, offset #1: %d\n", (int)freq_clock0, 0);
        printf("output frequency #2: %d, offset #2: %d\n", (int)freq_clock1, 90);
        
        /// random colours
        red = rand() & 0xFF;
        green = rand() & 0xFF;
        blue = rand() & 0xFF;
        setColor(red, green, blue);
        _delay_ms(200);

        // detect at port 39
        bool flag = (PINA & _BV(PINA1)) >> PINA1;
        if(flag) {
            PORTD |= (_BV(PORTD5) | _BV(PORTD4));
        } else {
            PORTD &= ~(_BV(PORTD5) | _BV(PORTD4));
        }
    }
    return 0;
}

