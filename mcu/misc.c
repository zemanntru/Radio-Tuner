#include "libraries/misc.h"

incr_t incr;
int arr_incr[6];
char msg_str[128];
bool changed;
bool scroll;
bool input_mode;
bool radio_mode;
uint32_t LO_freq_A;
uint32_t LO_freq_B;

static uint32_t _gcd(uint32_t a, uint32_t b);

void set_msg(bool mode)
{
    if(input_mode == 0) {
        printf("%d", arr_incr[5]);
        printf("%d.", arr_incr[4]);
        printf("%d", arr_incr[3]);
        printf("%d", arr_incr[2]);
        printf("%d", arr_incr[1]);
        printf("%d ", arr_incr[0]);
        printf("%d ", incr);
        printf(mode ? "TX" : "RX");
        printf("\n");
    }
    char msga[20] = "LO freq:\0";
    char msgb[20];
    char msgc[20] = "         \0";

    int cnt = 0;
    if(arr_incr[5])
        msgb[cnt++] = '0' + arr_incr[5];
    else
        msgb[cnt++] = ' ';
    
    msgb[cnt++] = '0' + arr_incr[4];
    msgb[cnt++] = '.';
    msgb[cnt++] = '0' + arr_incr[3];
    msgb[cnt++] = '0' + arr_incr[2];
    msgb[cnt++] = '0' + arr_incr[1];
    msgb[cnt++] = '0' + arr_incr[0];
    msgb[cnt++] = 'M';
    msgb[cnt++] = 'H';
    msgb[cnt++] = 'z';
    
    msgb[cnt++] = '\0';

    if(incr < 4) 
        msgc[6 - incr] = '^';
    else
        msgc[1] = '^';

    display_write((char*)msga, (char*)msgb, (char*)msgc, mode ? "Mode: TX\0" : "Mode: RX\0" );   
    changed = 0; 
}

void misc_init(bool mode)
{
    incr = ENCODER_STEP_10KHZ;
    for(int i = 0; i < 6; ++i)
        arr_incr[i] = 0;

    arr_incr[ENCODER_STEP_1MHZ] = 2;
    arr_incr[ENCODER_STEP_10MHZ] = 1;
    
    setup_PLL(SI5351_PLL_A, 36, 0, 1);
    set_LO_freq(12000000);
    enable_clocks(true);
    set_msg(mode);
    scroll = false;
    memset(msg_str, 0, sizeof(char)*128);
}

bool read_encoder()
{
    bool flag = 0;
    if(check_encoder())
    {
        if(is_pressed()) 
            scroll = !scroll;
            

        else if(is_moved()) 
        {
            if(scroll)
                incr = (incr + 1) % 5;
            else {
                int diff = get_diff();
                diff = (diff < 0 ? -1 : 1);

                //printf("difference: %d\n", diff);
                if(incr < ENCODER_STEP_1MHZ)
                    arr_incr[incr] = (arr_incr[incr] + diff + 10) % 10;
                else {
                    int rot = 8 + (arr_incr[ENCODER_STEP_10MHZ] * 10 +
                        arr_incr[ENCODER_STEP_1MHZ] + diff) % 8;

                    arr_incr[ENCODER_STEP_1MHZ] = rot % 10;
                    arr_incr[ENCODER_STEP_10MHZ] = rot / 10;
                } 
                set_LO_freq(
                        arr_incr[5] * 10000000 +
                        arr_incr[4] * 1000000 +
                        arr_incr[3] * 100000 +
                        arr_incr[2] * 10000 +
                        arr_incr[1] * 1000 +
                        arr_incr[0] * 100
                );
            }
            flag = 1;
        }
    }
    return flag;
}

void set_LO_freq(uint32_t freq)
{
    uint32_t fvco = 900000000, div, num, denom, gcd, rdiv;

    //freq = freq / 100 * 100;
    //rdiv = choose_rdiv(&freq);
    div = fvco / freq;
    num = fvco % freq;
    denom = freq;

    gcd = _gcd(num, denom);
    num /= gcd;
    denom /= gcd;
    
    if(denom > 0xFFFFF)
    {
        num = (float)num / denom * 0xFFFFF;
        denom = 0xFFFFF;
    }

    setup_clock(SI5351_PLL_A, SI5351_PORT0, div, num, denom);
    setup_clock(SI5351_PLL_A, SI5351_PORT1, div, num, denom);
    set_phase(round((float)fvco / freq));
}

uint32_t _gcd(uint32_t a, uint32_t b)
{
    return (a == 0) ? b: _gcd(b % a, a);
}

void read_messages()
{
    #define mod(x) ((x + 128) % 128)
    if(msg_buffer[mod(msg_back - 1)] == '\n' || msg_buffer[mod(msg_back - 1)] == '\r')
    {
        int len = mod(msg_back - msg_front);
        int cnt = 0;
        for(int i = 0, idx; i < len; ++i) {
            idx = mod(msg_front + i);
            if(!(msg_buffer[idx] == '\n' || msg_buffer[idx] == '\r')) 
                msg_str[cnt++] = msg_buffer[idx];
            
        }
        msg_str[cnt] = '\0';
        msg_buffer[msg_back - 1] = '\0';
        msg_front = msg_back;
        
    }
}