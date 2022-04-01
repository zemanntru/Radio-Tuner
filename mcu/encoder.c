#include "libraries/encoder.h"

byte read_register(byte addr)
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

word read_register16(byte addr)
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

void write_register(byte addr, byte data)
{
    twi_start();
    twi_MT_SLA_W(QWIIC_TWIST_ADDR);
    twi_MT_write(addr);
    twi_MT_write(data);
    twi_stop();
}


byte is_pressed()
{
    byte status = read_register(TWIST_STATUS);
    write_register(TWIST_STATUS, status & ~_BV(TWIST_PRESS_STATUS));
    return (status & _BV(TWIST_PRESS_STATUS)) > 0;
}

word get_count()
{
    return read_register16(TWIST_COUNT);
}