c.out: chip8.c
	gcc -g -o c.out chip8.c

makech8: makech8.c
	gcc -O2 -o makech8 makech8.c