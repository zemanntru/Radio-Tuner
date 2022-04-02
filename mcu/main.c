
#include "libraries/uart.h"
#include "libraries/encoder.h"
#include "libraries/si5351.h"

int main(void) 
{
    CLKPR = 0;
    /* initialize all necessary libraries */
    usart_init();
    twi_init();
    si5351_init();
    setup_PLL(SI5351_PLL_A, 36, 0, 1);
    clock_gen(SI5351_PLL_A, SI5351_PORT1, SI5351_MULTISYNTH_DIV_8, 0, 1);
    while(1)
    {
        printf("frequency: %d\n", freq_plla);
        _delay_ms(2000);
    }
    return 0;
}

