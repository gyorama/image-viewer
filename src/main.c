#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

void help(void) {
    puts("viewimg [OPTIONS] [PATH]\n\n"
         "An image viewer made for mostly educational purposes\n"
         "Controls:\n"
         "\tr        | resize the window to the size of the image (Defailt window size is 800x600)\n"
         "\tf        | Fullscreen the window");
}

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
                break;

            case '/':
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
            
            default:
                printf("Incorrect option: %s", argv[i]);
            }
        }
    }
    if (path[0] == '\0') {
        perror("Image path is empty\n");
        free(path);
        return 1;
    }

    // Initialize the SDL libraries
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
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
        free(path);
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
    SDL_RenderCopy(renderer, image, NULL, NULL);
    SDL_RenderPresent(renderer);

    bool isFullscreen = false;
    bool isTheSizeOfImage = false;

    while (!hasQuit) {
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    hasQuit = true;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.scancode == SDL_SCANCODE_F) {
                        SDL_SetWindowFullscreen(window, (isFullscreen) ? SDL_WINDOW_FULLSCREEN : 0); // set window to fullscreen if `fullscreen` is true  
                        isFullscreen = (isFullscreen) ? false : true; // Sidenote: I think I'm overusing the ? operator
                    } else if (event.key.keysym.scancode == SDL_SCANCODE_R) {
                        // inconsistency in ? and if statement in this and previous part is because this part requires 2 arguments
                        if (isTheSizeOfImage) {
                            SDL_SetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);
                            SDL_RenderClear(renderer);
                            SDL_RenderCopy(renderer, image, NULL, NULL);
                            SDL_RenderPresent(renderer);
                            isTheSizeOfImage = false;
                            continue;
                        }
                        SDL_SetWindowSize(window, imgSurf->w, imgSurf->h);
                        SDL_RenderClear(renderer);
                        SDL_RenderCopy(renderer, image, NULL, NULL);
                        SDL_RenderPresent(renderer);
                        isTheSizeOfImage = true;
                    }
                default:
                    break;
            }
        }


        SDL_Delay(16);
    }

    free(path);
    SDL_DestroyTexture(image);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}