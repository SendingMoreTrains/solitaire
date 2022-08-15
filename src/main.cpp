#include "SDL.h"
#include "SDL_image.h"
#include <stdio.h>

#include "screen.cpp"

const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 300;

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        Screen screen("Hello World!", SCREEN_WIDTH, SCREEN_HEIGHT);
        if (screen.init())
        {
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
            SDL_Texture* spritesheet = IMG_LoadTexture(screen.getRenderer(), ASSETS_PATH"card_spritesheet.png");

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
                        if(e.key.keysym.sym == SDLK_EQUALS)
                        {
                            screen.scaleUp();
                        }
                        if(e.key.keysym.sym == SDLK_MINUS)
                        {
                            screen.scaleDown();
                        }
                    }
                }

                SDL_SetRenderDrawColor(screen.getRenderer(), 0x00, 0x00, 0xFF, 0xFF);
                SDL_RenderClear(screen.getRenderer());

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
                SDL_RenderCopy(screen.getRenderer(), spritesheet, &srcRect, &destRect);

                SDL_RenderPresent(screen.getRenderer());
            }

            SDL_DestroyTexture(spritesheet);
        }
    }

    SDL_Quit();

    return 0;
}