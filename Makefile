EMU = src/emulator
COMP = src/compiler

chip8-sdl: $(EMU)/chip8-sdl.c $(EMU)/chip8_fun.c
	gcc $(EMU)/chip8-sdl.c $(EMU)/chip8_fun.c \
		-o chip8-sdl $(shell sdl2-config --cflags --libs)

makech8: $(COMP)/makech8.c
	gcc -g -o makech8 $(COMP)/makech8.c