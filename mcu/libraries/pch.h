#ifndef PCH_HEADER
#define PCH_HEADER

#define BAUD 2400
#define F_CPU 1000000

#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <util/twi.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <math.h>

extern FILE uart_output;
extern FILE uart_input;

typedef unsigned char byte;
typedef unsigned short word;
#endif