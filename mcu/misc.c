#include "libraries/misc.h"

incr_t incr;
uint32_t curFreq;
uint32_t arr_incr[5];
uint32_t prevRot;

static uint32_t _gcd(uint32_t a, uint32_t b);

void misc_init()
{
    /* set global variables */
    curFreq = DEFAULT_FREQ;
    incr = ENCODER_STEP_10KHZ;
    for(int i = 0; i < 5; ++i)
        arr_incr[i] = 0;

    arr_incr[ENCODER_STEP_1MHZ] = 12;
    set_count(20000);
    set_LO_freq(12000000);
    /*
     * initialize output ports (not initiated are input ports)
     * Pin 40 --> DDA0  (LCD reset)
     * Pin 19 --> DDD5  (TXEN EN)
     * Pin 18 --> DDD4  (board LED)
     */

    DDRA = _BV(DDA0);
    DDRD = _BV(DDD5) | _BV(DDD4);
}

void read_encoder()
{
    if(is_pressed()) {
        set_count(20000);
        incr = (incr + 1) % 5;
    }
    else if(is_moved()) {
        uint32_t rot = get_count();
        if(incr != ENCODER_STEP_1MHZ)
            arr_incr[incr] = rot % 10;
        else
            arr_incr[incr] = 8 + rot % 9;

        set_LO_freq(arr_incr[4] * 1000000 +
                arr_incr[3] * 100000 +
                arr_incr[2] * 10000 +
                arr_incr[1] * 1000 +
                arr_incr[0] * 100
        );
    }
    /*
    if(arr_incr[4] < 10)
        printf("LO frequency:  %d", arr_incr[4]);
    else
        printf("LO frequency: %d", arr_incr[4]);

    printf(",%d", arr_incr[3]);
    printf("%d", arr_incr[2]);
    printf("%d", arr_incr[1]);
    printf(",%d00\n", arr_incr[0]);
    */
    printf("%d", arr_incr[4]);
    printf(",%d", arr_incr[3]);
    printf("%d", arr_incr[2]);
    printf("%d", arr_incr[1]);
    printf(",%d00\n", arr_incr[0]);
    /*switch(incr)
    {
        case ENCODER_STEP_1MHZ:
            printf("               ^\n"); break;
        case ENCODER_STEP_100KHZ:
            printf("                 ^\n"); break;
        case ENCODER_STEP_10KHZ:
            printf("                  ^\n"); break;
        case ENCODER_STEP_1KHZ:
            printf("                   ^\n"); break;
        case ENCODER_STEP_100HZ:
            printf("                     ^\n"); break;
    }*/

}

void set_LO_freq(uint32_t freq)
{

    uint32_t div, gcd, num, denom, fvco, scale;


    div = FREQ_800MHZ / freq;
    if(div & 1) ++div;


    scale = div;
    fvco = freq * div;

    div = fvco / SI5351_XTAL_FREQ;
    num = fvco % SI5351_XTAL_FREQ;
    denom = SI5351_XTAL_FREQ;

    gcd = _gcd(num, denom);
    num /= gcd;
    denom /= gcd;

    /*
    printf("div: %u\n", div);
    printf("num: %u\n", num);
    printf("denom: %u\n", denom);
    printf("gcd: %u\n", gcd);
    printf("scale: %u\n", scale);
    */

    if(denom > 0xFFFFF)
    {
        num = (float)num / denom * 0xFFFFFF;
        denom = 0xFFFFF;
    }
    setup_PLL(SI5351_PLL_A, div, num, denom);
    setup_clock(SI5351_PLL_A, SI5351_PORT0, div, 0, 1);
    setup_clock(SI5351_PLL_A, SI5351_PORT1, div, 0, 1);

    set_phase(SI5351_PORT1, scale);

    enable_clocks(true);
}

uint32_t _gcd(uint32_t a, uint32_t b)
{
    return (a == 0) ? b: _gcd(b % a, a);
}
