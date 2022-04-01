MCU=atmega324a
CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS=-Wall -Os -mmcu=${MCU} 
TARGET=main
SRCS=main.c uart.c

all:
	${CC} ${CFLAGS} -o ${TARGET}.bin ${SRCS}
	${OBJCOPY} -j .text -j .data -O ihex ${TARGET}.bin ${TARGET}.hex  
clean:
	rm -f *.bin *.hex *.o *.elf
