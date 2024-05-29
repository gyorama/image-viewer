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

void help(void) {
    puts("viewimg [OPTIONS] [PATH]\n\n"
         "An image viewer made for mostly educational purposes\n"
         "OPTIONS:\n"
         "\t-r        | resize the window to the size of the image (Defailt window size is 800x600)\n"
         "\t-h        | print this and exit");
}

void getArgFunc(char arg) {}

int main(int argc, char const *argv[]) {
    char *path = calloc(400, sizeof(char));

    if (!path) {
        perror("Failed to allocate space to the path string");
        return 1;
    }

    char *backupPath = path;

    if (argc < 2) {
        help();
        return 1;
    } else if (argc >= 2) {
        for (int8_t i = 1; i < argc; i+=1) {
            switch (argv[i][0]) {
            case '-':
                getArgFunc(argv[i][1]);
                break;
            
            default:
                if (strlen(path) < strlen(argv[i])) {
                    path = realloc(path, strlen(argv[i])*sizeof(char));
                    if (!path) {
                        perror("Failed to resize space to the path string");
                        free(backupPath);
                        return 1;
                    }
                }
                strcpy(path, argv[i]);
                break;
            }
        }
    }
    if (path[0] == '\0') {
        perror("Image path is empty\nYou must've specified a flag without the image path\nOr an error occured");
        free(path);
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
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (!renderer) {
        printf("Renderer creation error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    bool hasQuit = false;
    SDL_Event event;

    SDL_Surface *imgSurf = IMG_Load(path);

    if (!imgSurf) {
        fprintf(stderr, "Image error: %s\n", SDL_GetError());
        free(path);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *image = SDL_CreateTextureFromSurface(renderer, imgSurf);
    SDL_FreeSurface(imgSurf);

    free(path);

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
    IMG_Quit();
    SDL_Quit();

    return 0;
}