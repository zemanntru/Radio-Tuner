#include "libraries/encoder.h"

static byte read(byte addr);
static word read16(byte addr);
static void write(byte addr, byte data);
static void write16(byte addr, word data);
static void write24(byte addr, uint32_t data);

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

word read16(byte addr)
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


void write(byte addr, byte data)
{
    twi_start();
    twi_MT_SLA_W(QWIIC_TWIST_ADDR);
    twi_MT_write(addr);
    twi_MT_write(data);
    twi_stop();
}

void write16(byte addr, word data)
{
    twi_start();
    twi_MT_SLA_W(QWIIC_TWIST_ADDR);
    twi_MT_write(addr);
    twi_MT_write(data & 0xFF);
    twi_MT_write(data >> 8);
    twi_stop();
}

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

void setColor(byte red, byte green, byte blue)
{
    write24(TWIST_RED, (uint32_t)red << 16 | (uint32_t)green << 8 | blue);
}


byte is_pressed()
{
    byte status = read(TWIST_STATUS);
    write(TWIST_STATUS, status & ~_BV(TWIST_PRESS_STATUS));
    return (status & _BV(TWIST_PRESS_STATUS)) > 0;
}

byte is_moved()
{
    byte status = read(TWIST_STATUS);
    write(TWIST_STATUS, status & ~_BV(TWIST_MOVED_STATUS));
    return (status & _BV(TWIST_MOVED_STATUS)) > 0;
}

word get_count()
{
    return read16(TWIST_COUNT);
}

void set_count(word cnt) 
{
    write16(TWIST_COUNT, cnt);
}


bool check_encoder()
{
    byte ret;
    twi_start();
    ret = twi_MT_SLA_W(QWIIC_TWIST_ADDR);
    twi_stop();
    return ret == TW_MT_SLA_ACK;
}

