CC = avr-gcc
MCU = atmega324a
CFLAGS = -Wall -Os -mmcu=${MCU} 
SRCS = uart.c twi.c encoder.c main.c
OBJS = $(patsubst %.c,%.o,$(SRCS))
OBJCOPY = avr-objcopy
TARGET = main

%.o: %.cc
	${CC} ${CFLAGS} -c -g $< -o $@

all: ${OBJS}
	${CC} ${CFLAGS} $^ -o ${TARGET}.bin 
	${OBJCOPY} -j .text -j .data -O ihex ${TARGET}.bin ${TARGET}.hex

clean:
	rm -f *.o *.hex *.bin *.elf