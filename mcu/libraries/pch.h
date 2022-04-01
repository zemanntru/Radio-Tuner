#define BAUD 2400
#define F_CPU 1000000

#ifndef PCH_HEADER
#define PCH_HEADER

#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <stdlib.h>
#include <stdio.h>

extern FILE uart_output;
extern FILE uart_input;

#endif