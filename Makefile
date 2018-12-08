# the compiler: gcc for C program, define as g++ for C++
CC = gcc

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g

all: myport monitor portMaster vessel

myport: myport.o global.o
	$(CC) $(CFLAGS) myport.o global.o -lm -o exe/myport

vessel: vessel.o global.o
	$(CC) $(CFLAGS) vessel.o global.o -lm -o exe/vessel

portMaster: portMaster.o global.o
	$(CC) $(CFLAGS) portMaster.o global.o -lm -o exe/portMaster

monitor: monitor.o global.o
	$(CC) $(CFLAGS) monitor.o global.o -lm -o exe/monitor

vessel.o: src/vessel.c
	$(CC) $(CFLAGS) -c src/vessel.c

portMaster.o: src/portMaster.c
	$(CC) $(CFLAGS) -c src/portMaster.c

monitor.o: src/monitor.c
	$(CC) $(CFLAGS) -c src/monitor.c

myport.o: src/myport.c
	$(CC) $(CFLAGS) -c src/myport.c

global.o: src/global.c
	$(CC) $(CFLAGS) -c src/global.c

clean:
	rm -fv *.o exe/*
	rm -f tmp/*

cunit:
	sudo apt-get install libcunit1 libcunit1-doc libcunit1-dev