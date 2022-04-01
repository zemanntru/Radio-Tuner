
#include "libraries/uart.h"


int main(void) 
{
    CLKPR = 0;
    usart_init();
    char str[100];
    while(1)
    {
        scanf("%s", str);
        printf("you typed in: %s\n", str);
    }
    return 0;
}

