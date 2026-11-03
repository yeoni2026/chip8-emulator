.PHONY: all

EMU = src/emulator
COMP = src/compiler

all: lest emu makech8

lest: $(COMP)/lest.c $(COMP)/lexer.c
	gcc -g $(COMP)/lest.c $(COMP)/lexer.c -o lest

emu: $(EMU)/emu.c $(EMU)/chip8.c
	gcc $(EMU)/emu.c $(EMU)/chip8.c \
		-o emu $(shell sdl2-config --cflags --libs)

makech8: $(COMP)/makech8.c
	gcc -g -o makech8 $(COMP)/makech8.c