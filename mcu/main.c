
#include "libraries/pch.h"
#include "libraries/twi.h"
#include "libraries/uart.h"
#include "libraries/si5351.h"
#include "libraries/misc.h"



int main(void) 
{
    /* initialize all necessary libraries */

    /*
     * if user select RX radio_mode, RX LED LIGHTS UP AND /TXEN is low 
     * initialize output ports (not initiated are input ports)
     * Pin 40 --> DDA0  (LCD reset)
     * Pin 19 --> DDD5  (TXEN EN)
     * Pin 18 --> DDD4  (board LED)
    */
    DDRA = _BV(DDA0);
    DDRD = _BV(DDD5) | _BV(DDD4);

    // power on LCD display 
    PORTA |= _BV(PORTA0);

    radio_mode = !((PINA & _BV(PINA1)) >> PINA1);
    input_mode = MECHANICAL_MODE;

    usart_init();
    twi_init();

    display_init();
    si5351_init();
    misc_init(radio_mode);
   
    while(1)
    {
        read_messages();
        if(!strcmp(msg_str, "mech")) {
            radio_mode = !((PINA & _BV(PINA1)) >> PINA1);
            input_mode = MECHANICAL_MODE;
            printf("Entering Mech Mode\n");

        } else if(!strcmp(msg_str, "comp")) {
            input_mode = SOFTWARE_MODE;
            printf("Entering Software Mode\n"); 
        }
        if(input_mode) 
        {
            if(!strcmp(msg_str, "TX0;")) {
                PORTD |= (_BV(PORTD4) | _BV(PORTD5));
                if(radio_mode == 1) {
                    radio_mode = 0;
                    changed = 1;
                }
            } else if(!strcmp(msg_str, "TX1;")) {
                PORTD &= ~(_BV(PORTD5)| _BV(PORTD4));
                if(radio_mode == 0) {
                    radio_mode = 1;
                    changed = 1;
                } 
            } else if(!strcmp(msg_str, "TX;")) {
                printf(radio_mode ? "TX1;\n" : "TX0;\n");
            } else if(!strncmp(msg_str, "FA", 2)) {
                if(msg_str[11] == ';') {
                    LO_freq_A = 
                        100000000 * max(0, msg_str[2] - '0') +
                         10000000 * max(0, msg_str[3] - '0') +
                          1000000 * max(0, msg_str[4] - '0') +
                           100000 * max(0, msg_str[5] - '0') +
                            10000 * max(0, msg_str[6] - '0') +
                             1000 * max(0, msg_str[7] - '0') +
                              100 * max(0, msg_str[8] - '0') +
                               10 * max(0, msg_str[9] - '0') +
                                    max(0, msg_str[10] - '0');

                    if((8000000 <= LO_freq_A) && (LO_freq_A <= 16000000)) {
                        changed = 1;
                        arr_incr[5] = max(0, msg_str[3] - '0');
                        arr_incr[4] = max(0, msg_str[4] - '0');
                        arr_incr[3] = max(0, msg_str[5] - '0');
                        arr_incr[2] = max(0, msg_str[6] - '0');
                        arr_incr[1] = max(0, msg_str[7] - '0');
                        arr_incr[0] = max(0, msg_str[8] - '0');

                        set_LO_freq(LO_freq_A);
                    }
                
                } else if(msg_str[2] == ';'){
                    uint32_t div = LO_freq_A;
                    printf("FA");
                    printf("%d", div / 100000000);
                    div %= 100000000;
                    printf("%d", div / 10000000);
                    div %= 10000000;
                    printf("%d", div / 1000000);
                    div %= 1000000;
                    printf("%d", div / 100000);
                    div %= 100000;
                    printf("%d", div / 10000);
                    div %= 10000;
                    printf("%d", div / 1000);
                    div %= 1000;
                    printf("%d", div / 100);
                    div %= 100;
                    printf("%d", div / 10);
                    div %= 10;
                    printf("%d;\n", div);
                }
            } else if(!strncmp(msg_str, "FB", 2)) {
                if(msg_str[11] == ';') {
                    LO_freq_B = 
                        100000000 * max(0, msg_str[2] - '0') +
                         10000000 * max(0, msg_str[3] - '0') +
                          1000000 * max(0, msg_str[4] - '0') +
                           100000 * max(0, msg_str[5] - '0') +
                            10000 * max(0, msg_str[6] - '0') +
                             1000 * max(0, msg_str[7] - '0') +
                              100 * max(0, msg_str[8] - '0') +
                               10 * max(0, msg_str[9] - '0') +
                                    max(0, msg_str[10] - '0');

                } else if(msg_str[2] == ';'){
                    uint32_t div = LO_freq_B;
                    printf("FB");
                    printf("%d", div / 100000000);
                    div %= 100000000;
                    printf("%d", div / 10000000);
                    div %= 10000000;
                    printf("%d", div / 1000000);
                    div %= 1000000;
                    printf("%d", div / 100000);
                    div %= 100000;
                    printf("%d", div / 10000);
                    div %= 10000;
                    printf("%d", div / 1000);
                    div %= 1000;
                    printf("%d", div / 100);
                    div %= 100;
                    printf("%d", div / 10);
                    div %= 10;
                    printf("%d;\n", div);
                }
            } else if(!strcmp(msg_str, "IF;")) {
                    uint32_t div = LO_freq_A;
                    printf("000");
                    printf("%d", div / 100000000);
                    div %= 100000000;
                    printf("%d", div / 10000000);
                    div %= 10000000;
                    printf("%d", div / 1000000);
                    div %= 1000000;
                    printf("%d", div / 100000);
                    div %= 100000;
                    printf("%d", div / 10000);
                    div %= 10000;
                    printf("%d", div / 1000);
                    div %= 1000;
                    printf("%d", div / 100);
                    div %= 100;
                    printf("%d", div / 10);
                    div %= 10;
                    printf("%d0000000000000;\n", div);
            }
        } else {
            if((PINA & _BV(PINA1)) >> PINA1) {
                PORTD |= (_BV(PORTD4) | _BV(PORTD5));
                if(radio_mode == 1) {
                    radio_mode = 0;
                    changed = 1;
                } 
            } else {
                PORTD &= ~(_BV(PORTD5)| _BV(PORTD4));
                if(radio_mode == 0) {
                    radio_mode = 1;
                    changed = 1;
                }
            }
            changed |= read_encoder();
        }
        if(changed) set_msg(radio_mode);
        memset(msg_str, 0, sizeof(char)*128);
    }
    return 0;
}