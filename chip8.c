#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

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

static const uint8_t font[80] = {
    0xF0,0x90,0x90,0x90,0xF0,  // 0
    0x20,0x60,0x20,0x20,0x70,  // 1
    0xF0,0x10,0xF0,0x80,0xF0,  // 2
    0xF0,0x10,0xF0,0x10,0xF0,  // 3
    0x90,0x90,0xF0,0x10,0x10,  // 4
    0xF0,0x80,0xF0,0x10,0xF0,  // 5
    0xF0,0x80,0xF0,0x90,0xF0,  // 6
    0xF0,0x10,0x20,0x40,0x40,  // 7
    0xF0,0x90,0xF0,0x90,0xF0,  // 8
    0xF0,0x90,0xF0,0x10,0xF0,  // 9
    0xF0,0x90,0xF0,0x90,0x90,  // A
    0xE0,0x90,0xE0,0x90,0xE0,  // B
    0xF0,0x80,0x80,0x80,0xF0,  // C
    0xE0,0x90,0x90,0x90,0xE0,  // D
    0xF0,0x80,0xF0,0x80,0xF0,  // E
    0xF0,0x80,0xF0,0x80,0x80   // F
};

int load_rom(chip8* c, const char* path);
uint16_t fetch(chip8* c);
void execute(chip8* c, uint16_t opcode);
void print_screeen(chip8* c);

int main(int argc, char *argv[]){
    chip8 *c = malloc(sizeof(chip8));
    if (c == NULL){
        fprintf(stderr, "Fail to assign heap.\n");
        exit(EXIT_FAILURE);
    }

    memset(c, 0, sizeof(*c));
    
    char file_name[64];
    
    if (argc == 1){
        int menu;
        printf("== 프로그램 선택 ==\n1. chip8 로고\n2. IBM 로고\n3. corax+ 검사\n===================\n선택: ");
        scanf("%d", &menu);


        switch(menu){
            case 1:
                strcpy(file_name, "1-chip8-logo.ch8");
                break;
            case 2:
                strcpy(file_name, "2-ibm-logo.ch8");
                break;
            case 3:
                strcpy(file_name, "3-corax+.ch8");
                break;
            default:
                fprintf(stderr, "잘못된 번호를 입력했습니다.\n");
                exit(EXIT_FAILURE);
        }
    } else if (argc == 2){
        strcpy(file_name, argv[1]);    
    } else {
        fprintf(stderr, "Usage: %s or %s <file_name>", argv[0], argv[0]);
        exit(EXIT_FAILURE);
    }

    int res = load_rom(c, file_name);
    if (res == -1){
        fprintf(stderr, "Fail in load_rom.\n");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    FILE* fp = fopen("file_content.txt", "w");
    for (int i = 0; i < 300; ++i){
        //임시 종료용
        uint16_t prev_PC = c->PC;
        
        uint16_t opcode = fetch(c);
        execute(c, opcode);
        fprintf(fp, "%04X ", opcode);

        if (prev_PC == c->PC) break;
        if ((i + 1) % 10 == 0){
            fprintf(fp, "\n");
        }
    }
    fclose(fp);

    print_screeen(c);
    return 0;
}

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

uint16_t fetch(chip8* c){
    if (c->PC >= FILE_START + 4096){
        fprintf(stderr, "segmentation fault.\n");
        exit(EXIT_FAILURE);
    }
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
                memset(c->screen, 0, HEIGHT * WIDTH);
            } 
            else if (opcode == 0x00EE){
                c->SP--;
                c->PC = c->stack[c->SP];
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
        case 0xF:
            switch(nn){
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
            }
            break;
        default:
            printf("Fail. opcode: %04X\n", opcode);
    }
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