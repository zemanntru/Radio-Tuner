#include "libraries/twi.h"

void twi_init()
{
	TWSR = 0x01; 
	TWBR = 0x01;
	TWCR = _BV(TWEN);
}

void twi_start()
{
    TWCR = _BV(TWINT)|_BV(TWSTA)|_BV(TWEN);
    while (!(TWCR & _BV(TWINT)));
}

void twi_repeat_start()
{
    TWCR = _BV(TWINT)|_BV(TWSTA)|_BV(TWEN);
    while (!(TWCR & _BV(TWINT)));
}

void twi_stop()
{
	TWCR = _BV(TWINT)|_BV(TWSTO)|_BV(TWEN); 
}

void twi_MT_SLA_W(byte addr)
{
    TWDR = (addr << 1) | TW_WRITE;
    TWCR = _BV(TWINT)|_BV(TWEN);                       
    while (!(TWCR & _BV(TWINT)));
}

void twi_MR_SLA_R(byte addr)
{
    TWDR = (addr << 1) | TW_READ;
    TWCR = _BV(TWINT)|_BV(TWEN);
    while (!(TWCR & _BV(TWINT)));
}

void twi_MT_write(byte data)
{
    TWDR = data;
    TWCR = _BV(TWINT)|_BV(TWEN);
    while (!(TWCR & _BV(TWINT)));
}

byte twi_MR_read_ACK()
{
    TWCR = _BV(TWINT)|_BV(TWEN)|_BV(TWEA);
    while (!(TWCR & _BV(TWINT)));
    return TWDR;
}

byte twi_MR_read_NACK()
{
    TWCR = _BV(TWINT)|_BV(TWEN);
    while (!(TWCR & _BV(TWINT)));
    return TWDR;
}