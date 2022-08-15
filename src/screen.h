#ifndef SOLITAIRE_SCREEN_H
#define SOLITAIRE_SCREEN_H

#include "SDL.h"

class Screen
{
public:
    Screen(const char* title, int game_width, int game_height);
    ~Screen();

    bool init();

    SDL_Renderer* getRenderer();

    void setScale(int scale);
    void scaleUp();
    void scaleDown();
private:
    const char* _title;
    int _window_scale;
    int _game_width, _game_height;
    SDL_Window* _window;
    SDL_Renderer* _renderer;
};

#endif //SOLITAIRE_SCREEN_H
