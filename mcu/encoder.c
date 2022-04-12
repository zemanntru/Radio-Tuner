#include "libraries/encoder.h"

static byte read(byte addr);
static int16_t read16(byte addr);
static void write(byte addr, byte data);
static void write16(byte addr, word data);
static void write24(byte addr, uint32_t data);

/*
 * extract 8-bit data based on register address
 * 
 * General format for reading a register:
 * 1. start twi process
 * 2. send device address
 * 3. send internal register address
 * 3. reset twi process
 * 4. read from device address
 * 5. stop twi process
 */

byte read(byte addr)
{
    byte ret;
    twi_start();
    twi_MT_SLA_W(QWIIC_TWIST_ADDR);
    twi_MT_write(addr);
    twi_repeat_start();
    twi_MR_SLA_R(QWIIC_TWIST_ADDR);
    ret = twi_MR_read_NACK();
    twi_stop();
    return ret;
}

/*
 * extract 16-bit data based on register address
 */

int16_t read16(byte addr)
{
    byte msb, lsb;
    twi_start();
    twi_MT_SLA_W(QWIIC_TWIST_ADDR);
    twi_MT_write(addr);
    twi_repeat_start();
    twi_MR_SLA_R(QWIIC_TWIST_ADDR);
    lsb = twi_MR_read_ACK();
    msb = twi_MR_read_NACK();
    twi_stop();
    return ((word)msb << 8) | lsb;
}

/*
 * write 8-bit data based on register address
 * General format for writing to a register
 * 1. start twi process
 * 2. send device address
 * 3. send internal register address
 * 4. send data
 * 5. stop twi process
 */

void write(byte addr, byte data)
{
    twi_start();
    twi_MT_SLA_W(QWIIC_TWIST_ADDR);
    twi_MT_write(addr);
    twi_MT_write(data);
    twi_stop();
}

/*
 * write 16-bit data based on register address
 */

void write16(byte addr, word data)
{
    twi_start();
    twi_MT_SLA_W(QWIIC_TWIST_ADDR);
    twi_MT_write(addr);
    twi_MT_write(data & 0xFF);
    twi_MT_write(data >> 8);
    twi_stop();
}

/*
 * write 24-bit data based on register address
 */

void write24(byte addr, uint32_t data)
{
    twi_start();
    twi_MT_SLA_W(QWIIC_TWIST_ADDR);
    twi_MT_write(addr);
    twi_MT_write(data >> 16);
    twi_MT_write(data >> 8);
    twi_MT_write(data & 0xFF);
    twi_stop();
}

void set_color(byte red, byte green, byte blue)
{
    /* write 24 bit data that combines the rgb values */
    write24(TWIST_RED, (uint32_t)red << 16 | (uint32_t)green << 8 | blue);
}


byte is_pressed()
{
    /*
     * read the bit indicating if knob is pressed
     * reset that register and return the value
     */

    byte status = read(TWIST_STATUS);
    write(TWIST_STATUS, status & ~_BV(TWIST_PRESS_STATUS));
    return (status & _BV(TWIST_PRESS_STATUS)) > 0;
}

byte is_moved()
{
    /*
     * read the bit indicating if knob is twisted
     * reset that register and return the value
     */
    byte status = read(TWIST_STATUS);
    write(TWIST_STATUS, status & ~_BV(TWIST_MOVED_STATUS));
    return (status & _BV(TWIST_MOVED_STATUS)) > 0;
}

word get_count()
{
    /* Get current rotation */
    return read16(TWIST_COUNT);
}

void set_count(word cnt) 
{
    /* Set current rotation */
    write16(TWIST_COUNT, cnt);
}

int16_t get_diff()
{
    /* Get rotation offset since last check */
    int16_t diff = read16(TWIST_DIFFERENCE);
    write16(TWIST_DIFFERENCE, 0);
    return diff;
}

bool check_encoder()
{
    /* check if encoder is connected */
    byte ret;
    twi_start();
    ret = twi_MT_SLA_W(QWIIC_TWIST_ADDR);
    twi_stop();
    return ret == TW_MT_SLA_ACK;
}
