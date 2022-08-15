#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

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
            SDL_Texture* spritesheet = IMG_LoadTexture(renderer, ASSETS_PATH"card_spritesheet.png");

//            screenSurface = SDL_GetWindowSurface(window);
//            SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
//
//            SDL_UpdateWindowSurface(window);

            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

            SDL_Rect srcRect = {};
            srcRect.x = 0;
            srcRect.y = 0;
            srcRect.w = 32;
            srcRect.h = 48;

            SDL_Rect destRect = {};
            destRect.x = 0;
            destRect.y = 0;
            destRect.w = 32 * 4;
            destRect.h = 48 * 4;
            SDL_RenderCopy(renderer, spritesheet, &srcRect, &destRect);

            SDL_RenderPresent(renderer);

            SDL_Delay(5000);

            SDL_DestroyTexture(spritesheet);
            SDL_DestroyRenderer(renderer);
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}