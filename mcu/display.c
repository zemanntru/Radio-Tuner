#include "libraries/display.h"

bool check_display()
{
    byte ret;
    twi_start();
    ret = twi_MT_SLA_W(SCREEN_ADDR);
    twi_stop();
    return ret == TW_MT_SLA_ACK;
    
}

void write_cmd(byte data)
{
    twi_start();
    twi_MT_SLA_W(SCREEN_ADDR);
    twi_MT_write(0x00);
    twi_MT_write(data);
    twi_stop();
}

void write_data(byte data)
{
    twi_start();
    twi_MT_SLA_W(SCREEN_ADDR);
    twi_MT_write(0x40);
    twi_MT_write(data);
    twi_stop();
}

void display_init()
{
    while(!check_display()) {
        printf("Error: LCD display not found!\n");
    }
    
    write_cmd(COMMAND_CLEAR_DISPLAY);
    write_cmd(COMMAND_8BIT_4LINES_NORMAL_RE1_IS0);
    write_cmd(COMMAND_NW);
    write_cmd(COMMAND_SEGMENT_BOTTOM_VIEW);
    write_cmd(COMMAND_BS1_1);
    write_cmd(COMMAND_8BIT_4LINES_RE0_IS1);
    write_cmd(COMMAND_BS0_1);
    write_cmd(COMMAND_FOLLOWER_CONTROL);
    write_cmd(COMMAND_POWER_BOOSTER_CONTRAST);
    write_cmd(COMMAND_SET_CONTRAST_1010);
    write_cmd(COMMAND_8BIT_4LINES_RE0_IS0);
    write_cmd(COMMAND_DISPLAY_ON_CURSOR_OFF_BLINK_OFF);
}

void display_write(char* msga, char* msgb, char* msgc, char* msgd)
{
    if(check_display()) {
        write_cmd(COMMAND_CLEAR_DISPLAY);

        write_cmd(COMMAND_SET_CURSOR_LINE_1);
        for(word cnt = 0; msga[cnt] != '\0' && cnt < 10; )
            write_data(msga[cnt++]);

        write_cmd(COMMAND_SET_CURSOR_LINE_2);
        for(word cnt = 0; msgb[cnt] != '\0' && cnt < 10; )
            write_data(msgb[cnt++]);

        write_cmd(COMMAND_SET_CURSOR_LINE_3);
        for(word cnt = 0; msgc[cnt] != '\0' && cnt < 10; )
            write_data(msgc[cnt++]);

        write_cmd(COMMAND_SET_CURSOR_LINE_4);
        for(word cnt = 0; msgd[cnt] != '\0' && cnt < 10; )
            write_data(msgd[cnt++]);
    }
}