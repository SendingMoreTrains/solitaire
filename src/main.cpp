#include "SDL.h"
#include "SDL_image.h"
#include <stdio.h>

const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 300;

int main(int argc, char** argv) {
    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        window = SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        }
        else
        {
            SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (renderer == NULL)
            {
                printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
            }
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
            SDL_Texture* spritesheet = IMG_LoadTexture(renderer, ASSETS_PATH"card_spritesheet.png");

            bool quit = false;
            SDL_Event e;

            while(!quit)
            {
                while(SDL_PollEvent(&e) != 0)
                {
                    if(e.type == SDL_QUIT)
                    {
                        quit = true;
                    }

                    if(e.type == SDL_KEYDOWN)
                    {
                        if(e.key.keysym.sym == SDLK_EQUALS && (e.key.keysym.mod & KMOD_SHIFT))
                        {
                            float currScaleX{}, currScaleY{};
                            SDL_RenderGetScale(renderer, &currScaleX, &currScaleY);
                            ++currScaleX;
                            ++currScaleY;

                            SDL_SetWindowSize(window, (int)currScaleX * SCREEN_WIDTH, (int)currScaleY * SCREEN_HEIGHT);
                            SDL_RenderSetScale(renderer, currScaleX, currScaleY);
                        }
                        if(e.key.keysym.sym == SDLK_MINUS)
                        {
                            float currScaleX{}, currScaleY{};
                            SDL_RenderGetScale(renderer, &currScaleX, &currScaleY);
                            if(currScaleX > 1 && currScaleY > 1)
                            {
                                --currScaleX;
                                --currScaleY;

                                SDL_SetWindowSize(window, (int)currScaleX * SCREEN_WIDTH, (int)currScaleY * SCREEN_HEIGHT);
                                SDL_RenderSetScale(renderer, currScaleX, currScaleY);
                            }
                        }
                    }
                }

                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);

                SDL_Rect srcRect = {};
                srcRect.x = 0;
                srcRect.y = 0;
                srcRect.w = 32;
                srcRect.h = 48;

                SDL_Rect destRect = {};
                destRect.x = 0;
                destRect.y = 0;
                destRect.w = 32;
                destRect.h = 48;
                SDL_RenderCopy(renderer, spritesheet, &srcRect, &destRect);

                SDL_RenderPresent(renderer);
            }

            SDL_DestroyTexture(spritesheet);
            SDL_DestroyRenderer(renderer);
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}