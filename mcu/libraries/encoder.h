#ifndef TWIST_HEADER
#define TWIST_HEADER

#include "pch.h"
#include "twi.h"

#define QWIIC_TWIST_ADDR 0x3F

#define TWIST_ID 0x00
#define TWIST_STATUS 0x01 //2 - button clicked, 1 - button pressed, 0 - encoder moved
#define TWIST_VERSION 0x02
#define TWIST_ENABLE_INTS 0x04 //1 - button interrupt, 0 - encoder interrupt
#define TWIST_COUNT 0x05
#define TWIST_DIFFERENCE 0x07
#define TWIST_LAST_ENCODER_EVENT 0x09 //Millis since last movement of knob
#define TWIST_LAST_BUTTON_EVENT 0x0B  //Millis since last press/release

#define TWIST_RED 0x0D
#define TWIST_GREEN 0x0E
#define TWIST_BLUE 0x0F

#define TWIST_CONNECT_RED 0x10 //Amount to change red LED for each encoder tick
#define TWIST_CONNECT_GREEN 0x12
#define TWIST_CONNECT_BLUE 0x14

#define TWIST_TURN_INT_TIMEOUT 0x16
#define TWIST_CHANGE_ADDRESS 0x18
#define TWIST_LIMIT 0x19

#define TWIST_PRESS_STATUS 2
#define TWIST_MOVED_STATUS 0

bool isConnected();

byte is_pressed();
byte is_moved();
word get_count();
void set_count(word cnt);
void set_color(byte red, byte green, byte blue);
int16_t get_diff();
bool check_encoder();

#endif