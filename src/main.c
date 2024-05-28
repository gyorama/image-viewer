#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

int WIN_WIDTH = 800;
int WIN_HEIGHT = 600;

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        puts("viewimg [PATH]\n\nThis is a (supposedly) lightweight image viewer");
        return 1;
    }

    // Initialize the SDL libraries
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        printf("SDL initialization error: %s\n", SDL_GetError());
        return 1;
    }
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_WEBP) &
        (IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_WEBP))) {
            printf("SDL initialization error: %s\n", SDL_GetError());
        return 1;
    };

    // Create the main window, "0" for no flags.
    SDL_Window *window = SDL_CreateWindow("Image viewer",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          WIN_WIDTH,
                                          WIN_HEIGHT,
                                          0);
    if (!window) {
        printf("Window creation error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (!renderer) {
        printf("Renderer creation error: %s\n", SDL_GetError());
        return 1;
    }

    bool hasQuit = false;
    SDL_Event event;

    SDL_Texture *image = IMG_LoadTexture(renderer, "/home/Moldy/txt");

    if (argc > 2) {
        for (int8_t i = 1; i < argc; i+=1) {
            switch (argv[i][1])
            {
            case 'f':
                SDL_QueryTexture(image, NULL, NULL, &WIN_WIDTH, &WIN_HEIGHT);
                SDL_SetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);
                break;
            
            default:
                break;
            }
        }
    }

    if (!image) {
        printf("Error loading image: %s", SDL_GetError);
        return 1;
    }

    int start, end;

    while (!hasQuit) {
        start = SDL_GetTicks();
        srand(time(NULL));
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    hasQuit = true;
                    break;
                default:
                    break;
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, image, NULL, NULL);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
        end = SDL_GetTicks();
        printf("FPS: %d\n", 1000/(end-start));
    }

    SDL_DestroyTexture(image);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);   
    SDL_Quit();

    return 0;
}