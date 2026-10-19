#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define FILE_START 0x200
#define WEIGHT 64
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
    uint8_t screen[HEIGHT][WEIGHT];
    uint16_t key_pad;
} chip8;

int load_rom(chip8* c, const char* path);
void step(chip8* c);
uint16_t fetch(chip8* c);
void execute(chip8* c, uint16_t opcode);

int main(){
    chip8 *c = malloc(sizeof(chip8));
    if (c == NULL){
        fprintf(stderr, "Fail to assign heap.\n");
        exit(EXIT_FAILURE);
    }

    memset(c, 0, sizeof(*c));

    int res = load_rom(c, "2-ibm-logo.ch8");
    if (res == -1){
        fprintf(stderr, "Fail in load_rom.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 5; ++i){
        step(c);
    }
    return 0;
}

int load_rom(chip8 *c, const char* path){
    FILE* fp = fopen(path, "rb");
    if (fp == NULL) return -1;

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (size <= 0 || size > 3584) {
        fclose(fp);
        return -1;
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

void step(chip8* c){
    uint16_t opcode = fetch(c);
    execute(c, opcode);
    printf("opcode=%04X  PC=%04X  I=%04X\n", opcode, c->PC, c->I);
    for (int i = 0; i < 16; ++i){
       printf("V[%X]=%02X  ", i, c->V[i]); 
    }
    printf("\n");
}

uint16_t fetch(chip8* c){
    uint16_t opcode = c->memory[c->PC];
    opcode <<= 8;
    opcode |= c->memory[c->PC + 1];

    c->PC += 2;
    return opcode;
}

void execute(chip8* c, uint16_t opcode){
    uint8_t x = (opcode >> 8) & 0xF;
    uint8_t y = (opcode >> 4) & 0xF;
    uint8_t n = opcode & 0xF;
    uint8_t nn = opcode & 0xFF;
    uint16_t nnn = opcode & 0xFFF;

    switch(opcode >> 12){
        case 0x0:
            if (opcode == 0x00E0){
                memset(c->screen, 0, HEIGHT * WEIGHT);
            }
            break;
        case 0x1:
            c->PC = nnn;
            break;
        case 0x6:
            c->V[x] = nn;
            break;
        case 0x7:
            c->V[x] += nn;
            break;
        case 0xA:
            c->I = nnn;
            break;
    }
}
