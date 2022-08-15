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

                int font_index{ 0 };
                std::vector<RenderedText> test_texts;
                test_texts.reserve(7);

                std::string font_paths_8[4]
                    {
                        "res/8-BIT WONDER.TTF",
                        "res/Arcadepix Plus.ttf",
                        "res/gameovercre1.ttf",
                        "res/superstar_memesbruh03.ttf",
                    };
                for (auto& font_path : font_paths_8)
                {
                    OutlineFont temp{ font_path.c_str(), 16, 1 };
                    SDL_Surface* text_surface{ temp.render_outlined_text("Testing 1 2 3, FreeCell") };

                    rect area{ 0, 0, text_surface->w, text_surface->h };
                    SDL_Texture* texture = renderContext.create_texture(text_surface);

                    test_texts.push_back(RenderedText {area, texture});

                    SDL_FreeSurface(text_surface);
                }

                std::string font_paths_18[1] { "res/8-bit-pusab.ttf" };
                for (auto& font_path : font_paths_18)
                {
                    OutlineFont temp{ font_path.c_str(), 18, 1 };
                    SDL_Surface* text_surface{ temp.render_outlined_text("Testing 1 2 3, FreeCell") };

                    rect area{ 0, 0, text_surface->w, text_surface->h };
                    SDL_Texture* texture = renderContext.create_texture(text_surface);

                    test_texts.push_back(RenderedText {area, texture});

                    SDL_FreeSurface(text_surface);
                }

                std::string font_paths_20[1] { "res/UpheavalPro.ttf" };
                for (auto& font_path : font_paths_20)
                {
                    OutlineFont temp{ font_path.c_str(), 20, 1 };
                    SDL_Surface* text_surface{ temp.render_outlined_text("Testing 1 2 3, FreeCell") };

                    rect area{ 0, 0, text_surface->w, text_surface->h };
                    SDL_Texture* texture = renderContext.create_texture(text_surface);

                    test_texts.push_back(RenderedText {area, texture});

                    SDL_FreeSurface(text_surface);
                }

                font_index = 0;
                current_text = test_texts[font_index];


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

                    if (inputState.keys.get_state(SDLK_n).was_pressed)
                    {
                        ++font_index;

                        if (font_index >= (int)test_texts.size())
                            font_index = 0;

                        current_text = test_texts[font_index];
                    }

                    if (inputState.keys.get_state(SDLK_p).was_pressed)
                    {
                        --font_index;

                        if (font_index < 0)
                            font_index += (int)test_texts.size();

                        current_text = test_texts[font_index];
                    }

                    solitaire.render(&renderContext);

                    // Font Test
                    {

                        rect dst_rect{ 20, 240, current_text.area.w, current_text.area.h };
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
