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

void send_quit_event()
{
    SDL_Event quit_event{};
    quit_event.type = SDL_QUIT;
    SDL_PushEvent(&quit_event);
}

#include "engine/engine.h"

#include "solitaire.cpp"

#include "scenes/scene.h"

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

                Solitaire solitaire(&renderContext, new FreeCell());

                SceneManager manager(&renderContext);
                manager.set_scene(new GameScene(&manager, new FreeCell()));


#if BENCHMARK
                Uint32 target_tick = SDL_GetTicks() + 1000;
                int frame_count = 0;
#endif

                bool quit = false;
                SDL_Event e;

                // OutlineFont chosen_font{ &renderContext, "res/Arcadepix Plus.ttf", 16, 1 };
                // current_text = chosen_font.render_outlined_text("Testing 1 2 3, FreeCell");

                current_text = renderContext.create_rendered_text("Testing 1 2 3, FreeCell");
                SDL_Texture* overlay = renderContext.create_overlay();


                solitaire.start_game();

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

                    if (inputState.keys.get_state(SDLK_r).was_pressed)
                    {
                        solitaire.reset();
                    }

                    // solitaire.update(&inputState);

                    // solitaire.render(&renderContext);

                    // renderContext.render_full_screen(overlay);
                    // renderContext.render_text(current_text, vec2 {20, 100});

                    manager.update(&inputState);
                    manager.render();

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

    TTF_Quit();
    SDL_Quit();

    return 0;
}
