#include <stdio.h>
#include <vector>
#include <iostream>

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
    SDL_Renderer* getRenderer();

    void renderSprite(SDL_Rect *src_rect, SDL_Rect *dest_rect);
private:
    int _spr_width;
    int _spr_height;
    SDL_Texture* _sheet;
    SDL_Renderer* _renderer;
};

SpriteSheet::SpriteSheet(SDL_Renderer* renderer, const char* filepath, int sprite_width, int sprite_height)
        : _renderer{ renderer }
        , _sheet{ IMG_LoadTexture(renderer, filepath) }
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

SDL_Texture* SpriteSheet::getTexture() { return _sheet; }

SDL_Renderer* SpriteSheet::getRenderer() { return _renderer; }

void SpriteSheet::renderSprite(SDL_Rect* src_rect, SDL_Rect* dest_rect)
{
    SDL_RenderCopy(_renderer, _sheet, src_rect, dest_rect);
}


class Sprite
{
public:
    Sprite(SpriteSheet* src_sheet, int sheet_col, int sheet_row);
    void render(vec2 pos);
    vec2 getSize();
private:
    SpriteSheet* _sheet;
    SDL_Rect _src_rect;
    SDL_Rect _dst_rect;
};

Sprite::Sprite(SpriteSheet* src_sheet, int sheet_col, int sheet_row)
    : _sheet{ src_sheet }
    , _src_rect{ _sheet->getSpriteSource(sheet_col, sheet_row) }
    , _dst_rect{ SDL_Rect{ 0, 0, _src_rect.w, _src_rect.h } }
{}

void Sprite::render(vec2 pos)
{
    _dst_rect.x = pos.x;
    _dst_rect.y = pos.y;
    _sheet->renderSprite(&_src_rect, &_dst_rect);
}

vec2 Sprite::getSize() { return vec2{ _src_rect.w, _src_rect.h }; }

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
    Card(SpriteSheet* src_sheet, Suit suit, int rank, vec2 pos);

    CardColor getColor();
    void render();

    vec2 getSize();
    vec2 getPosition();
    void setPosition(vec2 pos);

    int getRank() { return _rank; }
private:
    Suit _suit;
    int _rank;
    Sprite _sprite;
    vec2 _pos;
};

Card::Card(SpriteSheet* src_sheet, Suit suit, int rank, vec2 pos = {0, 0})
    : _suit{ suit }
    , _rank{rank }
    , _sprite(src_sheet, _suit, (_rank - 1))
    , _pos{ pos }
{}

CardColor Card::getColor()
{
    if (_suit == Suit::Spades || _suit == Suit::Clubs)
    {
        return CardColor::Black;
    }

    return CardColor::Red;
}

vec2 Card::getSize() { return _sprite.getSize(); }
vec2 Card::getPosition() { return _pos; }
void Card::setPosition(vec2 pos) { _pos = pos; }
void Card::render() { _sprite.render(_pos); }


int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        Screen screen("Hello World!", SCREEN_WIDTH, SCREEN_HEIGHT);
        screen.setScale(3);
        if (screen.init())
        {
            SpriteSheet spriteSheet(screen.getRenderer(), ASSETS_PATH"card_spritesheet.png", 32, 48);

//            Card card(&spriteSheet, Suit::Clubs, 12, { 20, 20 });

            std::vector<Card> cards{};
            cards.reserve(10);
            for (int i = 0; i < 10; ++i) {
                cards.emplace_back(&spriteSheet, Suit::Spades, i + 2, vec2 { 20, 20 + (i * 13) });
            }

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

                    if(e.type == SDL_MOUSEBUTTONDOWN)
                    {
                        auto x = e.button.x;
                        auto y = e.button.y;
                        std::cout << "X: " << x << "\nY: " << y << std::endl;
                        for(auto card_i = cards.rbegin(); card_i != cards.rend(); ++card_i)
                        {
                            vec2 pos{ card_i->getPosition() };
                            vec2 size{ card_i->getSize() };
                            if (x > pos.x && x < pos.x + size.x && y > pos.y && y < pos.y + size.y)
                            {
                                std::cout << "Rank: " << card_i->getRank() << "\n" << std::endl;
                                break;
                            }
                        }
                    }
                }

                SDL_SetRenderDrawColor(screen.getRenderer(), 22, 128, 17, 0xFF);
                SDL_RenderClear(screen.getRenderer());

                for(auto card_i = cards.begin(); card_i != cards.end(); ++card_i)
                {
                    card_i->render();
                }

                SDL_RenderPresent(screen.getRenderer());
            }
        }
    }

    SDL_Quit();

    return 0;
}