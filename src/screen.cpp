#include <stdio.h>
#include "screen.h"

Screen::Screen(const char *title, int game_width, int game_height)
        : _title{title}, _window_scale{ 1 }, _game_width{game_width}, _game_height{game_height}, _window{nullptr},
          _renderer{nullptr} {
}

Screen::~Screen() {
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
}

bool Screen::init() {
    _window = SDL_CreateWindow(_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _game_width * _window_scale,
                               _game_height * _window_scale, SDL_WINDOW_SHOWN);
    if (_window == nullptr) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    _renderer = SDL_CreateRenderer(_window, -1, (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
    if (_renderer == nullptr) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_RenderSetLogicalSize(_renderer, _game_width, _game_height);

    return true;
}

SDL_Renderer *Screen::getRenderer() {
    return _renderer;
}

void Screen::setScale(int scale) {
    _window_scale = scale;
    SDL_SetWindowSize(_window, _game_width * _window_scale, _game_height * _window_scale);
}

void Screen::scaleUp() { setScale(_window_scale + 1); }

void Screen::scaleDown() { if (_window_scale > 1) { setScale(_window_scale - 1); }}
