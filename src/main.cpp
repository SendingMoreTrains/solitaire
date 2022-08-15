#include <stdio.h>
#include <memory>
#include <utility>

#include "SDL.h"
#include "SDL_image.h"

#include "screen.cpp"

const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 300;

struct vec2
{
    int x, y;
};

class SpriteSheet
{
public:
    SpriteSheet(SDL_Renderer* renderer, const char* filepath, int sprite_width, int sprite_height);
    ~SpriteSheet();

    SDL_Rect getSpriteSource(int spr_col, int spr_row);
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
    void render(SDL_Renderer* renderer, vec2 pos);
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

void Sprite::render(SDL_Renderer* renderer, vec2 pos)
{
    _dst_rect.x = pos.x;
    _dst_rect.y = pos.y;
    SDL_RenderCopy(renderer, _sheet->getTexture(), &_src_rect, &_dst_rect);
}

enum Suit
{
    Spades = 0,
    Diamonds = 1,
    Clubs = 2,
    Hearts = 3
};

enum CardColor
{
    Black,
    Red
};

class Card
{
public:
    Card(std::shared_ptr<SpriteSheet> src_sheet, Suit suit, int value);

    CardColor getColor();
    void render(SDL_Renderer* renderer);
    void render(SDL_Renderer* renderer, vec2 pos);
    void setPosition(vec2 pos);
private:
    Suit _suit;
    int _value;
    Sprite _sprite;
    vec2 _pos;
};

Card::Card(std::shared_ptr<SpriteSheet> src_sheet, Suit suit, int value)
    : _suit{ suit }
    , _value{ value }
    , _sprite(std::move(src_sheet), _suit, (_value - 1))
    , _pos{ 0, 0 }
{}

CardColor Card::getColor()
{
    if (_suit == Suit::Spades || _suit == Suit::Clubs)
    {
        return CardColor::Black;
    }

    return CardColor::Red;
}

void Card::setPosition(vec2 pos)
{
    _pos = pos;
}

void Card::render(SDL_Renderer* renderer)
{
    _sprite.render(renderer, _pos);
}

void Card::render(SDL_Renderer* renderer, vec2 pos)
{
    setPosition(pos);
    render(renderer);
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
            std::shared_ptr<SpriteSheet> spriteSheet = std::make_shared<SpriteSheet>(screen.getRenderer(), ASSETS_PATH"card_spritesheet.png", 32, 48);

            Card card(spriteSheet, Suit::Clubs, 12);
            card.setPosition({20, 10});

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

                card.render(screen.getRenderer());

                SDL_RenderPresent(screen.getRenderer());
            }
        }
    }

    SDL_Quit();

    return 0;
}