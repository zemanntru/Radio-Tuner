#include "libraries/misc.h"

incr_t incr;
uint32_t arr_incr[6];
bool changed;
bool scroll;

static uint32_t _gcd(uint32_t a, uint32_t b);

void set_msg(bool mode)
{
    char msga[20] = "LO freq:\0";
    char msgb[20];
    int cnt = 0;
    if(arr_incr[5])
        msgb[cnt++] = '0' + arr_incr[5];
    
    msgb[cnt++] = '0' + arr_incr[4];


    msgb[cnt++] = '0' + arr_incr[3];
    

    msgb[cnt++] = '0' + arr_incr[2];

    msgb[cnt++] = '0' + arr_incr[1];
   
    msgb[cnt++] = '.';

    msgb[cnt++] = '0' + arr_incr[0];

  
    msgb[cnt++] = 'K';
    msgb[cnt++] = 'H';
    msgb[cnt++] = 'z';
    
    msgb[cnt++] = '\0';

    display_write((char*)msga, (char*)msgb, mode ? "Mode: TX\0" : "Mode: RX\0" );   
    changed = 0; 
}

void misc_init(bool mode)
{
    incr = ENCODER_STEP_10KHZ;
    for(int i = 0; i < 6; ++i)
        arr_incr[i] = 0;

    arr_incr[ENCODER_STEP_1MHZ] = 2;
    arr_incr[ENCODER_STEP_10MHZ] = 1;

    set_count(20000);
    set_LO_freq(12000000);
    set_msg(mode);
    scroll = false;
}

bool read_encoder()
{
    bool flag = 0;
    if(check_encoder())
    {
        /*
        if(is_pressed()) {
            scroll = !scroll;
            if(!scroll) set_count(20000);
        }
        */
        if(is_pressed()) {
            incr = (incr + 1) % 5;
            set_count(20000);
        }

        if(is_moved()) {
            
            uint32_t rot = get_count();
            if(incr == ENCODER_STEP_1KHZ) {
                if(arr_incr[ENCODER_STEP_1KHZ] |  
                arr_incr[ENCODER_STEP_1KHZ] |
                arr_incr[ENCODER_STEP_1KHZ] |
                arr_incr[ENCODER_STEP_1KHZ])
                arr_incr[incr] = rot % 10;
                else
                arr_incr[incr] = 8 + rot % 2;
            }
            else if(incr < ENCODER_STEP_1MHZ)
                arr_incr[incr] = rot % 10;
            else {
                rot = 8 + rot % 9;
                arr_incr[ENCODER_STEP_1MHZ] = rot % 10;
                arr_incr[ENCODER_STEP_10MHZ] = rot / 10;

                if(arr_incr[ENCODER_STEP_10MHZ] == 1 && arr_incr[ENCODER_STEP_1MHZ] == 6)
                for(int i = 0; i < 4; ++i)
                    arr_incr[i] = 0;
            }
            printf("%d", arr_incr[5]);
            printf("%d", arr_incr[4]);
            printf("%d", arr_incr[3]);
            printf("%d", arr_incr[2]);
            printf("%d.", arr_incr[1]);
            printf("%d\n", arr_incr[0]);

            set_LO_freq(
                    arr_incr[5] * 10000000 +
                    arr_incr[4] * 1000000 +
                    arr_incr[3] * 100000 +
                    arr_incr[2] * 10000 +
                    arr_incr[1] * 1000 +
                    arr_incr[0] * 100
            );
            flag = 1;
            
        }
    }
    return flag;
}

void set_LO_freq(uint32_t freq)
{

    uint32_t div, gcd, num, denom, fvco, scale;

    // freq = 100
    // div = 8
    div = FREQ_800MHZ / freq;
    if(div & 1) ++div;

    // scale = 8
    // fvco = 800
    scale = div;
    fvco = freq * div;

    //printf("div 1: %u\n", div);

    
    div = fvco / SI5351_XTAL_FREQ;
    num = fvco % SI5351_XTAL_FREQ;
    denom = SI5351_XTAL_FREQ;

    // div = 

    /*
    printf("div 2: %u\n", div);
    printf("num 2: %u\n", num);
    printf("denom 2: %u\n", denom);
    */
    
    gcd = _gcd(num, denom);
    num /= gcd;
    denom /= gcd;

    //printf("div: %u\n", div);
    /*printf("num: %u\n", num);
    printf("denom: %u\n", denom);
    printf("gcd: %u\n", gcd);
    printf("scale: %u\n", scale);*/

    if(denom > 0xFFFFF)
    {
        num = (float)num / denom * 0xFFFFF;
        denom = 0xFFFFF;
    }

    setup_PLL(SI5351_PLL_A, div, num, denom);

    setup_clock(SI5351_PLL_A, SI5351_PORT0, scale, 0, 1);
    setup_clock(SI5351_PLL_A, SI5351_PORT1, scale, 0, 1);
    set_phase((byte)div);
    _delay_ms(500);
    enable_clocks(true);
}

uint32_t _gcd(uint32_t a, uint32_t b)
{
    return (a == 0) ? b: _gcd(b % a, a);
}
