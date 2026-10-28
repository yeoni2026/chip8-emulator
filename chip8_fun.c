#include "chip8_fun.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int load_rom(chip8 *c, const char* path){
    FILE* fp = fopen(path, "rb");
    if (fp == NULL) {
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (size <= 0 || size > 3584) {
        fclose(fp);
        return -1;
    }

    //메모리의 폰트 시작 주소에 폰트 내용 복사
    for (int i = 0; i < 16; ++i){
        for (int j = 0; j < 5; ++j){
            c->memory[FONT_START + 5 * i + j] = font[5 * i + j];
        }
    }

    //메모리의 파일 시작 주소에 파일 내용 복사
    if (fread(c->memory + FILE_START, sizeof(uint8_t), size, fp) < size) {
        fclose(fp);
        return -1;
    }
    fclose(fp);

    //PC 값을 파일 시작 주소로 초기화
    c->PC = FILE_START;
    return 0;
}

int fetch(chip8* c, uint16_t *opcode){
    if (c->PC >= FILE_START + 4096){
        fprintf(stderr, "segmentation fault.\n");
        return -1;
    }
    uint16_t oc = c->memory[c->PC];
    oc <<= 8;
    oc |= c->memory[c->PC + 1];

    c->PC += 2;
    *opcode = oc;
    return 0;
}

int execute(chip8* c, uint16_t opcode){
    uint8_t x = (opcode >> 8) & 0xF;
    uint8_t y = (opcode >> 4) & 0xF;
    uint8_t n = opcode & 0xF;
    uint8_t nn = opcode & 0xFF;
    uint16_t nnn = opcode & 0xFFF;

    switch(opcode >> 12){
        case 0x0:
            switch(nn){
                case 0xE0:
                    memset(c->screen, 0, HEIGHT * WIDTH);
                    break;
                case 0xEE:
                    c->SP--;
                    c->PC = c->stack[c->SP];
                    break;
                default:
                    printf("Fail. opcode: %04X\n", opcode);
                    return -1;
            }
            break;
        case 0x1:
            c->PC = nnn;
            break;
        case 0x2:
            c->stack[c->SP] = c->PC;
            c->PC = nnn;
            c->SP++;
            break;
        case 0x3:
            if (c->V[x] == nn){
                c->PC += 2;
            }
            break;
        case 0x4:
            if (c->V[x] != nn){
                c->PC += 2;
            }
            break; 
        case 0x5:
            if (c->V[x] == c->V[y]){
                c->PC += 2;
            }
            break;
        case 0x6:
            c->V[x] = nn;
            break;
        case 0x7:
            c->V[x] += nn;
            break;
        case 0x8:
            switch(n){
                case 0x0:
                    c->V[x] = c->V[y];
                    break;
                case 0x1:
                    c->V[x] |= c->V[y];
                    break;
                case 0x2:
                    c->V[x] &= c->V[y];
                    break;
                case 0x3:
                    c->V[x] ^= c->V[y];
                    break;
                case 0x4: {
                    uint8_t flag = (uint16_t)c->V[x] + c->V[y] > 0xFF;
                    c->V[x] += c->V[y];
                    c->V[0xF] = flag;
                    break;
                }
                case 0x5: {
                    uint8_t flag = c->V[x] >= c->V[y];
                    c->V[x] -= c->V[y];
                    c->V[0xF] = flag;
                    break;
                }
                case 0x6: {
                    uint8_t flag = c->V[x] & 0x1;
                    c->V[x] >>= 1;
                    c->V[0xF] = flag;
                    break;
                }
                case 0x7: {
                    uint8_t flag = c->V[y] >= c->V[x];
                    c->V[x] = c->V[y] - c->V[x];
                    c->V[0xF] = flag;
                    break;
                }
                case 0xE: {
                    uint8_t flag = (c->V[x] >> 7) & 0x1;
                    c->V[x] <<= 1;
                    c->V[0xF] = flag;
                    break;
                }
                default:
                    printf("Fail. opcode: %04X\n", opcode);
                    return -1;
            }
            break;
        case 0x9:
            if (c->V[x] != c->V[y]){
                c->PC += 2;
            }
            break;
        case 0xA: 
            c->I = nnn;
            break;
        case 0xB:
            c->PC = (nnn + c->V[0]) & 0xFFF; // 메모리 범위 초과 방지
            break;
        case 0xC:
            c->V[x] = (rand() % 256) & nn;
            break; 
        case 0xD:
            c->V[0xF] = 0;
            for (int row = 0; row < n; ++row){
                if (c->V[y] + row >= HEIGHT) break;
                uint8_t sprite_byte = c->memory[c->I + row];

                for (int col = 0; col < 8; ++col){
                    if (c->V[x] + col >= WIDTH) break;
                    uint8_t sprite_bit = (sprite_byte >> (7 - col)) & 1;
                    if (c->screen[c->V[y] + row][c->V[x] + col] && sprite_bit) c->V[0xF] = 1;
                    c->screen[c->V[y] + row][c->V[x] + col] ^= sprite_bit;
                }
            }
            break;
        case 0xE:
            switch(nn){
                case 0x9E:
                    if (c->key_pad >> (c->V[x] & 0xF) & 0x1) c->PC += 2;
                    break;
                case 0xA1:
                    if (!(c->key_pad >> (c->V[x] & 0xF) & 0x1)) c->PC += 2;
                    break;
                default:
                    printf("Fail. opcode: %04X\n", opcode);
                    return -1;
            }
            break;
        case 0xF:
            switch(nn){
                case 0x07:
                    c->V[x] = c->delay_timer;
                    break;
                case 0x0A: {
                    int key_pressed = 0;

                    for (int i = 0; i < 16; ++i) {
                        if ((c->key_pad >> i) & 1) {
                            c->V[x] = i;
                            key_pressed = 1;
                            break;
                        }
                    }

                    if (!key_pressed)
                        c->PC -= 2;
                }
                case 0x15:
                    c->delay_timer = c->V[x];
                    break;
                case 0x18:
                    c->sound_timer = c->V[x];
                    break;
                case 0x1E:
                    c->I += c->V[x];
                    break;
                case 0x29:
                    c->I = FONT_START + (c->V[x] & 0xF) * 5;
                    break;
                case 0x33:
                    c->memory[c->I] = (c->V[x] / 100);
                    c->memory[c->I + 1] = (c->V[x] / 10) % 10;
                    c->memory[c->I + 2] = c->V[x] % 10;
                    break;
                case 0x55:
                    for (int i = 0; i <= x; ++i){
                        c->memory[c->I + i] = c->V[i];
                    }
                    break;
                case 0x65:
                    for (int i = 0; i <= x; ++i){
                        c->V[i] = c->memory[c->I + i];
                    }
                    break;
                default:
                    printf("Fail. opcode: %04X\n", opcode);
                    return -1;
            }
            break;
        default:
            printf("Fail. opcode: %04X\n", opcode);
            return -1;
    }
    return 0;
}

void print_screeen(chip8* c){
    for (int row = 0; row < HEIGHT; ++row){
        for (int col = 0; col < WIDTH; ++col){
            char symbol = c->screen[row][col] ? '#' : '.';
            printf("%c ", symbol);
        }
        printf("\n");
    }
}

void tick_timers(chip8 *c){
    if (c->delay_timer > 0) c->delay_timer -= 1;
    if (c->sound_timer > 0) c->sound_timer -= 1;
}

void extract_command(uint16_t opcode, int index){
    FILE* fp = fopen("content.txt", "a");
    if (fp == NULL) return;

    fprintf(fp, "%04X ", opcode);

    if ((index + 1) % 10 == 0){
        fprintf(fp, "\n");
    }
    fclose(fp);
}