CC = avr-gcc
MCU = atmega324a
CFLAGS = -Wall -Os -mmcu=${MCU} 
SRCS = uart.c twi.c encoder.c main.c si5351.c misc.c
OBJS = $(patsubst %.c,%.o,$(SRCS))
OBJCOPY = avr-objcopy
TARGET = main
PORT = /dev/ttyUSB0

%.o: %.cc
	${CC} ${CFLAGS} -c -g $< -o $@

all: ${OBJS}
	${CC} ${CFLAGS} $^ -o ${TARGET}.bin 
	${OBJCOPY} -j .text -j .data -O ihex ${TARGET}.bin ${TARGET}.hex
	
flash:
	avrdude -p ${MCU} -c ft232r -U flash:w:${TARGET}.hex:i -F -P ${PORT}

clean:
	rm -f *.o *.hex *.bin *.elf