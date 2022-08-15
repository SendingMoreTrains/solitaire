#include <stdio.h>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <iterator>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 300;

typedef SDL_Rect rect;

struct vec2 {
    int x, y;

    vec2 operator+(const vec2& right)
    {
        return vec2 { x + right.x, y + right.y };
    }

    void operator+=(const vec2& right)
    {
        this->x += right.x;
        this->y += right.y;
    }

    vec2 operator-(const vec2& right)
    {
        return vec2 { x - right.x, y - right.y };
    }

    void operator-=(const vec2& right)
    {
        this->x -= right.x;
        this->y -= right.y;
    }
};

SDL_Surface* CreateSDL_RGBA_Surface(int width, int height)
{
    Uint32 rmask, gmask, bmask, amask;
    /* SDL interprets each pixel as a 32-bit number, so our masks must depend
       on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    return SDL_CreateRGBSurface(0,width,height,32,rmask, gmask, bmask, amask);
}

#include "engine/graphics.cpp"
#include "engine/text.cpp"
#include "engine/input.cpp"

#include "nsolitaire.cpp"

struct RenderedText
{
    SDL_Rect area;
    SDL_Texture* texture;
};

RenderedText current_text{};


int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    } else {
        if (TTF_Init() < 0)
            printf("SDL_TTF could not initialize! SDL_Error: %s\n", TTF_GetError());
        else
        {
            RenderContext renderContext("Solitaire", SCREEN_WIDTH, SCREEN_HEIGHT, 3);
            if (renderContext.init())
            {
                InputState inputState{};

                Solitaire solitaire(&renderContext);

#if BENCHMARK
                Uint32 target_tick = SDL_GetTicks() + 1000;
                int frame_count = 0;
#endif

                bool quit = false;
                SDL_Event e;

                OutlineFont chosen_font{ "res/Arcadepix Plus.ttf", 16, 1 };
                {
                    SDL_Surface* text_surface{ chosen_font.render_outlined_text("Testing 1 2 3, FreeCell") };

                    rect area{ 0, 0, text_surface->w, text_surface->h };
                    SDL_Texture* texture = renderContext.create_texture(text_surface);

                    current_text = RenderedText {area, texture};

                    SDL_FreeSurface(text_surface);
                }


                // OVERLAY TEST - REMOVE THIS
                SDL_Texture* overlay;
                {
                    // Create a surface that will be the returned result
                    SDL_Surface* overlay_s = CreateSDL_RGBA_Surface(SCREEN_HEIGHT,SCREEN_WIDTH);

                    SDL_FillRect(overlay_s, NULL, SDL_MapRGBA(overlay_s->format, 0, 0, 0, 160));

                    overlay = renderContext.create_texture(overlay_s);
                    SDL_FreeSurface(overlay_s);
                }

                // GAME LOOP
                while (!quit)
                {
                    processFrameStart(&inputState);

                    while (SDL_PollEvent(&e) != 0)
                    {
                        if (e.type == SDL_QUIT)
                        {
                            quit = true;
                        }

                        // if (e.type == SDL_KEYDOWN) {
                        //   if (e.key.keysym.sym == SDLK_EQUALS) {
                        //     screen.scaleUp();
                        //   }
                        //   if (e.key.keysym.sym == SDLK_MINUS) {
                        //     screen.scaleDown();
                        //   }
                        // }

                        processSdlEvent(&inputState, &e);

                    }

                    solitaire.update(&inputState);

                    solitaire.render(&renderContext);

                    SDL_RenderCopy(renderContext._renderer, overlay, NULL, NULL);

                    // Font Test
                    {
                        rect dst_rect{ 20, 100, current_text.area.w, current_text.area.h };
                        SDL_RenderCopy(renderContext._renderer, current_text.texture, NULL, &dst_rect);
                    }

                    renderContext.present();

#if BENCHMARK
                    ++frame_count;

                    if (SDL_GetTicks() >= target_tick)
                    {
                        std::cout << "FPS: " << frame_count << std::endl;
                        frame_count = 0;
                        target_tick = SDL_GetTicks() + 1000;
                    }
#endif
                }
            }
        }

    }

    TTF_Init();
    SDL_Quit();

    return 0;
}
