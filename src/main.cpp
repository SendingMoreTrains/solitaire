#include <stdio.h>
#include <vector>
#include <iostream>

#include "SDL.h"
#include "SDL_image.h"

const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 300;

typedef SDL_Rect rect;

struct vec2 {
    int x, y;
};

#include "engine/graphics.cpp"
#include "engine/input.cpp"

#include "solitaire.cpp"

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    } else {
        RenderContext renderContext("Solitaire", SCREEN_WIDTH, SCREEN_HEIGHT, 2);
        if (renderContext.init()) {
            MouseState inputState;

            init(&renderContext);

            bool quit = false;
            SDL_Event e;

            while (!quit) {
                inputState.left.wasReleased = false;
                inputState.left.wasPressed = false;
                inputState.right.wasReleased = false;
                inputState.right.wasPressed = false;

                while (SDL_PollEvent(&e) != 0) {
                    if (e.type == SDL_QUIT) {
                        quit = true;
                    }

//                    if (e.type == SDL_KEYDOWN) {
//                        if (e.key.keysym.sym == SDLK_EQUALS) {
//                            screen.scaleUp();
//                        }
//                        if (e.key.keysym.sym == SDLK_MINUS) {
//                            screen.scaleDown();
//                        }
//                    }

                    processInput(&inputState, &e);
                }


                update(&inputState);

                render(&renderContext);
            }
        }
    }

    SDL_Quit();

    return 0;
}
