#include "libraries/si5351.h"

static void write(byte addr, byte data);
static byte read(byte addr);
static void enableSpreadSpectrum(bool enabled);
static void write_bulk(byte addr, byte* data, int len);

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
    byte status = 0;
    /* wait for device to start */
    do {
        status = read(SI5351_REGISTER_0_DEVICE_STATUS);
    } while (status >> 7 == 1);
    
    write(SI5351_REGISTER_183_CRYSTAL_INTERNAL_LOAD_CAPACITANCE, 
        (SI5351_CRYSTAL_LOAD_8PF & SI5351_CRYSTAL_LOAD_MASK) | 0b00010010);

    write(SI5351_REGISTER_16_CLK0_CONTROL, 0x80); 
    write(SI5351_REGISTER_17_CLK1_CONTROL, 0x80); 
    write(SI5351_REGISTER_18_CLK2_CONTROL, 0x80);
    write(SI5351_REGISTER_19_CLK3_CONTROL, 0x80);
    write(SI5351_REGISTER_20_CLK4_CONTROL, 0x80);
    write(SI5351_REGISTER_21_CLK5_CONTROL, 0x80);
    write(SI5351_REGISTER_22_CLK6_CONTROL, 0x80);
    
    enableSpreadSpectrum(false);
}

void enable_clocks(bool enabled)
{
    write(SI5351_REGISTER_3_OUTPUT_ENABLE_CONTROL, enabled ? 0x00 : 0xFF);
}

void setup_PLL(plldev_t pll, byte mult, uint32_t num, uint32_t denom)
{
    if( mult < 15 || mult > 90 ) {
        printf("PLL failed initialization: multiple not supported.\n");
        return;    
    }
    if( (denom == 0) || (denom > 0xFFFFF) ) {
        printf("PLL failed initialization: denom not in range.\n");
        return;
    }
    if( num > 0xFFFFF ) {
        printf("PLL failed initialization: num not in range.\n");
        return;
    }
    
    uint32_t p1, p2, p3;
    if(num == 0) {
        p1 = 128 * mult - 512;
        p2 = num;
        p3 = denom;
    } else {
        p1 = 128 * mult + (128 * (num / denom)) - 512;
        p2 = 128 * num - denom * (128 * (num / denom));
        p3 = denom;
    }
    byte addr = (pll == SI5351_PLL_A) ? SI5351_PLLA_ADDR : SI5351_PLLB_ADDR;

    write(addr, (p3 & 0x0000FF00) >> 8);
    write(addr + 1, (p3 & 0x000000FF));
    write(addr + 2, (p1 & 0x00030000) >> 16);
    write(addr + 3, (p1 & 0x0000FF00) >> 8);
    write(addr + 4, (p1 & 0x000000FF));
    write(addr + 5, ((p3 & 0x000F0000) >> 12) | ((p2 & 0x000F0000) >> 16));
    write(addr + 6, (p2 & 0x0000FF00) >> 8);
    write(addr + 7, (p2 & 0x000000FF));

    write(SI5351_REGISTER_177_PLL_RESET, (1 << 7) | (1 << 5));
}

void setup_clock(plldev_t pll, byte port, uint32_t div, uint32_t num, uint32_t denom)
{
    if( port > 2 ) return;
    if( div < 4 || div > 2048 ) return;
    if( (denom == 0) || (denom > 0xFFFFF) ) return;
    if( num > 0xFFFFF) return;

    uint32_t p1, p2, p3;

    if (num == 0) {
        p1 = 128 * div - 512;
        p2 = 0;
        p3 = denom;
    } else if (denom == 1){
        p1 = 128 * div + 128 * num - 512;
        p2 = 128 * num - 128;
        p3 = 1;
    } else {
        p1 = 128 * div + (128 * (num / denom)) - 512;
        p2 = 128 * num - denom * (128 * (num / denom));
        p3 = denom;
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
        (p3 & 0xFF00) >> 8,
        p3 & 0xFF,
        (p1 & 0x30000) >> 16, 
        (p1 & 0xFF00) >> 8,
        p1 & 0xFF,
        ((p3 & 0xF0000) >> 12) | ((p2 & 0xF0000) >> 16),
        (p2 & 0xFF00) >> 8,
        p2 & 0xFF
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

void set_phase(byte port, byte mult)
{
    mult = mult & 0xb01111111;
    switch(port) 
    {
        case SI5351_PORT0: 
            write(SI5351_REGISTER_165_CLK0_INITIAL_PHASE_OFFSET, mult);
            break;
        case SI5351_PORT1:
            write(SI5351_REGISTER_166_CLK1_INITIAL_PHASE_OFFSET, mult);
            break;
        case SI5351_PORT2:
            write(SI5351_REGISTER_167_CLK2_INITIAL_PHASE_OFFSET, mult);
            break;
    }

}
