.PHONY: all

EMU = src/emulator
COMP = src/compiler

all: emu makech8

emu: $(EMU)/emu.c $(EMU)/chip8.c
	gcc $(EMU)/emu.c $(EMU)/chip8.c \
		-o emu $(shell sdl2-config --cflags --libs)

makech8: $(COMP)/makech8.c
	gcc -g -o makech8 $(COMP)/makech8.c