LIBS= -lsfml-graphics -lsfml-window -lsfml-system -pthread
CC = g++ -std=c++17

SRCS = main.cpp pch.cpp graphics.cpp
OBJS = $(patsubst %.cpp,%.o,$(SRCS))

%.o: %.cc
	$(CC) -c -g $< -o $@ 

gui.bin: $(OBJS)
	$(CC) $^ -o $@ $(LIBS)
    
clean:
	rm -f *.o *.bin
	