#include "libraries/si5351.h"

static void write(byte addr, byte data);
static byte read(byte addr);
static void enableSpreadSpectrum(bool enabled);
static void write_bulk(byte addr, byte* data, int len);

uint32_t lastRdivValue[3];

void write(byte addr, byte data)
{
    twi_start();
    twi_MT_SLA_W(SI5351_ADDR);
    twi_MT_write(addr);
    twi_MT_write(data);
    twi_stop();
}

void write_bulk(byte addr, byte* data, int len)
{
    twi_start();
    twi_MT_SLA_W(SI5351_ADDR);
    twi_MT_write(addr);
    for(int i = 0; i < len; ++i)
        twi_MT_write(data[i]);

    twi_stop();
}


void set_phase(word mult) 
{
    mult &= 0b1111111;
    write(SI5351_REGISTER_165_CLK0_INITIAL_PHASE_OFFSET, 0);
    write(SI5351_REGISTER_166_CLK1_INITIAL_PHASE_OFFSET, mult);
    write(SI5351_REGISTER_177_PLL_RESET, (1 << 7) | (1 << 5));
}


byte read(byte addr)
{
    byte ret;
    twi_start();
    twi_MT_SLA_W(SI5351_ADDR);
    twi_MT_write(addr);
    twi_repeat_start();
    twi_MR_SLA_R(SI5351_ADDR);
    ret = twi_MR_read_NACK();
    twi_stop();
    return ret;
}

void enableSpreadSpectrum(bool enabled) {
    byte regval = read(SI5351_REGISTER_149_SPREAD_SPECTRUM_PARAMETERS);
    if(enabled) 
        regval |= 0x80;
    else 
        regval &= ~0x80;

    write(SI5351_REGISTER_149_SPREAD_SPECTRUM_PARAMETERS, regval);
}

void si5351_init()
{
    for(int i = 0; i < 3; ++i) lastRdivValue[i] = 0;
    byte status = 0;
    /* wait for device to start */
    do {
        status = read(SI5351_REGISTER_0_DEVICE_STATUS);
    } while (status >> 7 == 1);

    write(SI5351_REGISTER_183_CRYSTAL_INTERNAL_LOAD_CAPACITANCE, 
        (SI5351_CRYSTAL_LOAD_10PF & SI5351_CRYSTAL_LOAD_MASK) | 0b00010010);

    write(SI5351_REGISTER_16_CLK0_CONTROL, 0x80); 
    write(SI5351_REGISTER_17_CLK1_CONTROL, 0x80); 
    write(SI5351_REGISTER_18_CLK2_CONTROL, 0x80);
    write(SI5351_REGISTER_19_CLK3_CONTROL, 0x80);
    write(SI5351_REGISTER_20_CLK4_CONTROL, 0x80);
    write(SI5351_REGISTER_21_CLK5_CONTROL, 0x80);
    write(SI5351_REGISTER_22_CLK6_CONTROL, 0x80);
    write(SI5351_REGISTER_23_CLK7_CONTROL, 0x80);

    write(16, 0x0c);
	write(17, 0x0c);
	write(18, 0x0c);
	write(19, 0x0c);
	write(20, 0x0c);
	write(21, 0x0c);
	write(22, 0x0c);
	write(23, 0x0c);

    enableSpreadSpectrum(false);  
}

void enable_clocks(bool enabled)
{
    write(SI5351_REGISTER_3_OUTPUT_ENABLE_CONTROL, enabled ? 0x00 : 0xFF);
}

void setup_PLL(plldev_t pll, byte mult, uint32_t num, uint32_t denom)
{
    
    if( mult < 15 || mult > 90 ) return;    // multiple not supported
    if( (denom == 0) || (denom > 0xFFFFF) ) return;
    if( num > 0xFFFFF) return;

    uint32_t P1, P2, P3;

    if(num == 0) {
        P1 = 128 * mult - 512;
        P2 = num;
        P3 = denom;
    } else {
        P1 = (uint32_t)(128 * mult + floor(128 * ((float)num/ (float)denom)) - 512);
        P2 = (uint32_t)(128 * num - denom * floor(128 * ((float)num / (float)denom)));
        P3 = denom;
    }
    byte addr = (pll == SI5351_PLL_A) ? SI5351_PLLA_ADDR : SI5351_PLLB_ADDR;

    write(addr, (P3 & 0x0000FF00) >> 8);
    write(addr + 1, (P3 & 0x000000FF));
    write(addr + 2, (P1 & 0x00030000) >> 16);
    write(addr + 3, (P1 & 0x0000FF00) >> 8);
    write(addr + 4, (P1 & 0x000000FF));
    write(addr + 5, ((P3 & 0x000F0000) >> 12) | ((P2 & 0x000F0000) >> 16));
    write(addr + 6, (P2 & 0x0000FF00) >> 8);
    write(addr + 7, (P2 & 0x000000FF));
    
}

void setup_clock(plldev_t pll, byte port, uint32_t div, uint32_t num, uint32_t denom)
{
    if( port > 2 ) return;
    if( div < 4 || div > 2048 ) return;
    if( (denom == 0) || (denom > 0xFFFFF) ) return;
    if( num > 0xFFFFF) return;

    uint32_t P1, P2, P3;
    if (num == 0) {
        P1 = 128 * div - 512;
        P2 = 0;
        P3 = denom;
    } else if (denom == 1){
        P1 = 128 * div + 128 * num - 512;
        P2 = 128 * num - 128;
        P3 = 1;
    } else {
        P1 = (uint32_t)(128 * div + floor(128 * ((float)num/ (float)denom)) - 512);
        P2 = (uint32_t)(128 * num - denom * floor(128 * ((float)num / (float)denom)));
        P3 = denom;
    }
    byte addr = 0;
    switch(port) 
    {
        case SI5351_PORT0: 
            addr = SI5351_REGISTER_42_MULTISYNTH0_PARAMETERS_1;
            break;
        case SI5351_PORT1:
            addr = SI5351_REGISTER_50_MULTISYNTH1_PARAMETERS_1;
            break;
        case SI5351_PORT2:
            addr = SI5351_REGISTER_58_MULTISYNTH2_PARAMETERS_1;
            break;
    }

    byte buffer[8] = {
        (P3 & 0xFF00) >> 8,
        P3 & 0xFF,
        ((P1 & 0x30000) >> 16) | lastRdivValue[port],
        (P1 & 0xFF00) >> 8,
        P1 & 0xFF,
        ((P3 & 0xF0000) >> 12) | ((P2 & 0xF0000) >> 16),
        (P2 & 0xFF00) >> 8,
        P2 & 0xFF
    };

    write_bulk(addr, buffer, 8);

    byte clkCtrlReg = 0x0F;

    if( pll == SI5351_PLL_B )
        clkCtrlReg |= (1 << 5);

    if( num == 0 )
        clkCtrlReg |= (1 << 6);

    switch(port) 
    {
        case SI5351_PORT0: 
            write(SI5351_REGISTER_16_CLK0_CONTROL, clkCtrlReg);
            break;
        case SI5351_PORT1:
            write(SI5351_REGISTER_17_CLK1_CONTROL, clkCtrlReg);
            break;
        case SI5351_PORT2:
            write(SI5351_REGISTER_18_CLK2_CONTROL, clkCtrlReg);
            break;
    }
}

void reset_pll()
{
    write(SI5351_REGISTER_177_PLL_RESET, (1 << 7) | (1 << 5));
}

word choose_rdiv(uint32_t *freq)
{
    uint8_t r_div = SI5351_R_DIV_1;

    // Choose the correct R divider
               
    if(*freq >= 4000 && *freq < 8000) {
        r_div = SI5351_R_DIV_128;
        *freq *= 128;
    } else if(*freq >= 8000  && *freq < 16000) {
        r_div = SI5351_R_DIV_64;
        *freq *= 64;
    } else if(*freq >= 16000 && *freq < 32000) {
        r_div = SI5351_R_DIV_32;
        *freq *= 32;
    } else if(*freq >= 32000 && *freq < 64000) {
        r_div = SI5351_R_DIV_16;
        *freq *= 16;
    } else if(*freq >= 64000 && *freq < 128000) {
        r_div = SI5351_R_DIV_8;
        *freq *= 8;
    } else if(*freq >= 128000 && *freq < 256000) {
        r_div = SI5351_R_DIV_4;
        *freq *= 4;
     } else if(*freq >= 256000 && *freq < 512000) {
        r_div = SI5351_R_DIV_2;
        *freq *= 2;
    }
    return r_div;

}

void setup_rdiv(byte port, byte div)
{
    uint8_t Rreg = SI5351_REGISTER_44_MULTISYNTH0_PARAMETERS_3, regval;

    switch(port)
    {
        case SI5351_PORT0: 
            Rreg = SI5351_REGISTER_44_MULTISYNTH0_PARAMETERS_3;
            break;
        case SI5351_PORT1:
            Rreg = SI5351_REGISTER_52_MULTISYNTH1_PARAMETERS_3;
            break;
        case SI5351_PORT2:
            Rreg = SI5351_REGISTER_60_MULTISYNTH2_PARAMETERS_3;
            break;
    }

    regval = read(Rreg);
    regval &= 0x0F;
    uint8_t divider = div;
    divider &= 0x07;
    divider <<= 4;
    regval |= divider;
    lastRdivValue[port] = divider;
    write(Rreg, regval);
}