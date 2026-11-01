#pragma once

#include <stdint.h>

#define FONT_START 0x050
#define FILE_START 0x200
#define WIDTH 64
#define HEIGHT 32

typedef struct { //대략 몇 KB정도..
    uint8_t memory[4096];
    uint8_t V[16];
    uint16_t I;
    uint16_t PC;
    uint16_t stack[16];
    uint8_t SP;
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t screen[HEIGHT][WIDTH];
    uint16_t key_pad;
} chip8;

int load_rom(chip8* c, const char* path);
int fetch(chip8* c, uint16_t *opcode);
int execute(chip8* c, uint16_t opcode);
void tick_timers(chip8 *c);