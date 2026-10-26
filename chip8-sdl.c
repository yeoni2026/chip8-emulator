#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>
#include "chip8_fun.h"

void draw_screen(SDL_Renderer *ren, chip8 *c);

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
        }

        //step 10번 밟기.
        for (int i = 0; i < 10; ++i){
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

            extract_command(opcode, i);
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