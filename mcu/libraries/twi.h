#ifndef TWI_HEADER
#define TWI_HEADER

#include "pch.h"

void twi_init();
void twi_start();
void twi_repeat_start();
void twi_stop();
byte twi_MT_SLA_W(byte addr);
void twi_MR_SLA_R(byte addr);
void twi_MT_write(byte data);
byte twi_MR_read_ACK();
byte twi_MR_read_NACK();
byte twi_MT_SLA_W_check(byte addr);

#endif 