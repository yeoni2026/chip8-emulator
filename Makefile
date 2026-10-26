c.out: chip8.c
	gcc -g -o c.out chip8.c

makech8: makech8.c
	gcc -g -o makech8 makech8.c

chip8-sdl: chip8-sdl.c chip8_fun.c
	gcc chip8-sdl.c chip8_fun.c -o chip8-sdl $(shell sdl2-config --cflags --libs)