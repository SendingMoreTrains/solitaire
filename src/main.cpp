#include <stdio.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>

#include "SDL.h"
#include "SDL_image.h"

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

#include "engine/graphics.cpp"
#include "engine/input.cpp"

#include "nsolitaire.cpp"

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    } else {
        RenderContext renderContext("Solitaire", SCREEN_WIDTH, SCREEN_HEIGHT, 3);
        if (renderContext.init()) {
            InputState inputState{};

            Solitaire solitaire(&renderContext);

#if BENCHMARK
            Uint32 target_tick = SDL_GetTicks() + 1000;
            int frame_count = 0;
#endif

            bool quit = false;
            SDL_Event e;

            // GAME LOOP
            while (!quit) {
                processFrameStart(&inputState);

                while (SDL_PollEvent(&e) != 0) {
                    if (e.type == SDL_QUIT) {
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

    SDL_Quit();

    return 0;
}
