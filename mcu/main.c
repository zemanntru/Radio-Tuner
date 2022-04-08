
#include "libraries/pch.h"
#include "libraries/twi.h"
#include "libraries/uart.h"
#include "libraries/si5351.h"
#include "libraries/misc.h"

int main(void) 
{
    /* initialize all necessary libraries */

    /*
     * if user select RX mode, RX LED LIGHTS UP AND /TXEN is low 
     * initialize output ports (not initiated are input ports)
     * Pin 40 --> DDA0  (LCD reset)
     * Pin 19 --> DDD5  (TXEN EN)
     * Pin 18 --> DDD4  (board LED)
    */

    usart_init();
    twi_init();

    DDRA = _BV(DDA0);
    DDRD = _BV(DDD5) | _BV(DDD4);

    /* power on LCD display */
    PORTA |= _BV(PORTA0);
    
    byte mode;
    if((PINA & _BV(PINA1)) >> PINA1)
        mode = 0;
    else
        mode = 1;
  
    display_init();
    si5351_init();
    misc_init(mode);

    while(1)
    {
        if((PINA & _BV(PINA1)) >> PINA1) {
            PORTD |= _BV(PORTD4);
            PORTD &= ~(_BV(PORTD5));
            if(mode == 1) {
                mode = 0;
                changed = 1;
            } 
        } else {
            PORTD |= _BV(PORTD5);
            PORTD &= ~(_BV(PORTD4));
            if(mode == 0) {
                mode = 1;
                changed = 1;
            }
        }
        changed |= read_encoder();
        if(changed) set_msg(mode);
    }
    return 0;
}