
#include "libraries/uart.h"
#include "libraries/encoder.h"

int main(void) 
{
    CLKPR = 0;
    /* initialize all necessary libraries */
    usart_init();
    twi_init();
    while(1)
    {
        printf("number of turns: %u\n", get_count());
        printf("button pressed: %d\n", is_pressed());
        _delay_ms(2000);
    }
    return 0;
}

