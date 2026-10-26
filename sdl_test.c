#include <stdio.h>
#include <SDL2/SDL.h>

void draw_screen(SDL_Renderer *ren, chip8 *c);

int main(int argc, char *argv[]) {
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