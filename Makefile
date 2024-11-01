CC = m68k-palmos-gcc
CFLAGS = -O2 -Wall

all: PWordle.prc

PWordle.prc: PalmWordle bin.stamp
	build-prc PWordle.prc "Wordle" WRDL PalmWordle *.bin

PalmWordle: PalmWordle.o
	$(CC) $(CFLAGS) -o PalmWordle PalmWordle.o

PalmWordle.o: PalmWordle.c PalmWordle.h
	$(CC) $(CFLAGS) -c PalmWordle.c

bin.stamp: PalmWordle.rcp PalmWordle.h *.bmp
	pilrc PalmWordle.rcp

clean:
	-rm -f *.[oa] PalmWordle *.bin *.stamp
