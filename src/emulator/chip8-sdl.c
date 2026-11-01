#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>
#include "chip8_fun.h"

#define FRAME 10

void draw_screen(SDL_Renderer *ren, chip8 *c);
int key_to_chip8(SDL_Scancode sc);

int main(int argc, char *argv[]) {
    chip8 *c = malloc(sizeof(chip8));
    if (c == NULL){
        fprintf(stderr, "Fail to assign heap.\n");
        exit(EXIT_FAILURE);
    }

    memset(c, 0, sizeof(*c));
    
    char file_name[64];
    
    if (argc == 1){
        int menu;
        printf("== 프로그램 선택 ==\n1. chip8 로고\n2. IBM 로고\n3. corax+ 검사\n4. Dinorun 게임\n5. octojam 디자인\n===================\n선택: ");
        scanf("%d", &menu);

        switch(menu){
            case 1:
                strcpy(file_name, "programs/1-chip8-logo.ch8");
                break;
            case 2:
                strcpy(file_name, "programs/2-ibm-logo.ch8");
                break;
            case 3:
                strcpy(file_name, "programs/3-corax+.ch8");
                break;
            case 4:
                strcpy(file_name, "programs/dinorun.ch8");
                break;
            case 5:
                strcpy(file_name, "programs/octojam1title.ch8");
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
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("CHIP-8",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, SDL_WINDOW_RESIZABLE);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, 0);
    SDL_RenderSetLogicalSize(ren, 64, 32);

    int running = 1;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
            else if (e.type == SDL_KEYDOWN){
                int num = key_to_chip8(e.key.keysym.scancode);
                if (num != -1){
                    c->key_pad |= 0x1 << (num & 0xF);
                }
            }
            else if (e.type == SDL_KEYUP){
                int num = key_to_chip8(e.key.keysym.scancode);
                if (num != -1){
                    c->key_pad &= ~(0x1 << (num & 0xF));
                }
            }
        }

        //step 밟기.
        for (int i = 0; i < FRAME; ++i){
            uint16_t opcode;
            int res = fetch(c, &opcode);
            if (res == -1){
                running = 0;
                break;
            }
            res = execute(c, opcode);
            if (res == -1){
                running = 0;
                break;
            }
        }
        tick_timers(c);

        //스크린 그리기
        draw_screen(ren, c);

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}

void draw_screen(SDL_Renderer *ren, chip8 *c){
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);

    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    SDL_Rect r;
    r.w = 1;
    r.h = 1;
    
    for (int row = 0; row < 32; ++row){
        for (int col = 0; col < 64; ++col){
            if (c->screen[row][col]){
                r.x = col;
                r.y = row;
                SDL_RenderFillRect(ren, &r);
            }
        }
    }
}

int key_to_chip8(SDL_Scancode sc){
    switch(sc){
        case SDL_SCANCODE_1:
            return 0x1;
        case SDL_SCANCODE_2:
            return 0x2;
        case SDL_SCANCODE_3:
            return 0x3;
        case SDL_SCANCODE_4:
            return 0xC;
        case SDL_SCANCODE_Q:
            return 0x4;
        case SDL_SCANCODE_W:
            return 0x5;
        case SDL_SCANCODE_E:
            return 0x6;
        case SDL_SCANCODE_R:
            return 0xD;
        case SDL_SCANCODE_A:
            return 0x7;
        case SDL_SCANCODE_S:
            return 0x8;
        case SDL_SCANCODE_D:
            return 0x9;
        case SDL_SCANCODE_F:
            return 0xE;
        case SDL_SCANCODE_Z:
            return 0xA;
        case SDL_SCANCODE_X:
            return 0x0;
        case SDL_SCANCODE_C:
            return 0xB;
        case SDL_SCANCODE_V:
            return 0xF;
        default:
            return -1;
    }
}