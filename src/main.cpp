#include <stdio.h>
#include <memory>

#include "SDL.h"
#include "SDL_image.h"

#include "screen.cpp"

const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 300;

class SpriteSheet
{
public:
    SpriteSheet(SDL_Renderer* renderer, const char* filepath, int sprite_width, int sprite_height);
    ~SpriteSheet();

    SDL_Rect getSpriteSource(int spr_x, int spr_y);
    SDL_Texture* getTexture();
private:
    int _spr_width;
    int _spr_height;
    SDL_Texture* _sheet;
};

SpriteSheet::SpriteSheet(SDL_Renderer* renderer, const char* filepath, int sprite_width, int sprite_height)
        : _sheet{ IMG_LoadTexture(renderer, filepath) }
        , _spr_width{ sprite_width }
        , _spr_height{ sprite_height }
{}

SpriteSheet::~SpriteSheet()
{
    SDL_DestroyTexture(_sheet);
}

SDL_Rect SpriteSheet::getSpriteSource(int spr_col, int spr_row)
{
    return SDL_Rect{spr_col * _spr_width, spr_row * _spr_height, _spr_width, _spr_height };
}

SDL_Texture* SpriteSheet::getTexture()
{
    return _sheet;
}


class Sprite
{
public:
    Sprite(std::shared_ptr<SpriteSheet> src_sheet, int sheet_col, int sheet_row);
    void render(SDL_Renderer* renderer, int x, int y);
private:
    std::shared_ptr<SpriteSheet> _sheet;
    SDL_Rect _src_rect;
    SDL_Rect _dst_rect;
};

Sprite::Sprite(std::shared_ptr<SpriteSheet> src_sheet, int sheet_col, int sheet_row)
    : _sheet{std::move( src_sheet )}
    , _src_rect{_sheet->getSpriteSource(sheet_col, sheet_row) }
    , _dst_rect{ SDL_Rect{ 0, 0, _src_rect.w, _src_rect.h } }
{}

void Sprite::render(SDL_Renderer* renderer, int x, int y)
{
    _dst_rect.x = x;
    _dst_rect.y = y;
    SDL_RenderCopy(renderer, _sheet->getTexture(), &_src_rect, &_dst_rect);
}

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        Screen screen("Hello World!", SCREEN_WIDTH, SCREEN_HEIGHT);
        screen.setScale(2);
        if (screen.init())
        {
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

            std::shared_ptr<SpriteSheet> spriteSheet = std::make_shared<SpriteSheet>(screen.getRenderer(), ASSETS_PATH"card_spritesheet.png", 32, 48);
            Sprite sprite(spriteSheet, 0, 3);

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

                SDL_SetRenderDrawColor(screen.getRenderer(), 22, 128, 17, 0xFF);
                SDL_RenderClear(screen.getRenderer());

                sprite.render(screen.getRenderer(), 10, 10);

                SDL_RenderPresent(screen.getRenderer());
            }
        }
    }

    SDL_Quit();

    return 0;
}