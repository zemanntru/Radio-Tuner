#ifndef MISC_HEADER
#define MISC_HEADER

#include "pch.h"
#include "si5351.h"
#include "encoder.h"
#include "display.h"

#define DEFAULT_FREQ    12000000
#define MECHANICAL_MODE     0
#define HDSDR_MODE          1
#define GUI_MODE            2
#define max(a,b) ((a)<(b)?(b):(a))
#define min(a,b) ((a)>(b)?(b):(a))

typedef enum {
    ENCODER_STEP_100HZ = 0,
    ENCODER_STEP_1KHZ,
    ENCODER_STEP_10KHZ,
    ENCODER_STEP_100KHZ,
    ENCODER_STEP_1MHZ,
    ENCODER_STEP_10MHZ
} incr_t;

extern incr_t incr;
extern uint32_t curFreq;
extern int arr_incr[6];
extern bool changed;
extern byte input_mode;
extern bool radio_mode;
extern char msg_str[128];
extern uint32_t LO_freq_A;
extern uint32_t LO_freq_B;

void set_msg(bool mode);
void read_messages();
void misc_init(bool mode);
bool read_encoder();
void set_LO_freq(uint32_t freq);
void update_PLL();

#endif