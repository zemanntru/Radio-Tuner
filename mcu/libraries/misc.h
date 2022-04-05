#ifndef MISC_HEADER
#define MISC_HEADER

#include "pch.h"
#include "si5351.h"
#include "encoder.h"

#define DEFAULT_FREQ    12000000
#define FREQ_800MHZ     800000000

#define max(a,b) ((a)<(b)?(b):(a))
#define min(a,b) ((a)>(b)?(b):(a))

typedef enum {
    ENCODER_STEP_100HZ = 0,
    ENCODER_STEP_1KHZ,
    ENCODER_STEP_10KHZ,
    ENCODER_STEP_100KHZ,
    ENCODER_STEP_1MHZ,
} incr_t;

extern incr_t incr;
extern uint32_t curFreq;
extern uint32_t arr_incr[5];

void misc_init();
void read_encoder();
void set_LO_freq(uint32_t freq);


#endif