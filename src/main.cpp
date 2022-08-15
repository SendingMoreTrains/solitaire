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

struct Sprite {
    SDL_Texture* texture;
    rect src_rect;
};

class SpriteSheet {
public:
    SpriteSheet(SDL_Texture* texture, int sprite_width, int sprite_height) : _texture{texture},
                                                                             _spr_width{sprite_width},
                                                                             _spr_height{sprite_height} {}

    ~SpriteSheet() { SDL_DestroyTexture(_texture); };

    Sprite createSprite(int sprite_column, int sprite_row);

private:
    SDL_Texture* _texture;
    int _spr_width, _spr_height;
};

Sprite SpriteSheet::createSprite(int sprite_column, int sprite_row) {
    SDL_Rect src_rect{sprite_column * _spr_width, sprite_row * _spr_height, _spr_width, _spr_height};
    return Sprite{_texture, src_rect};
}

class RenderContext {
public:
    RenderContext(const char* title, int game_width, int game_height, int scale = 1) : _title{title},
                                                                                       _game_width{game_width},
                                                                                       _game_height{game_height},
                                                                                       _window_scale{scale},
                                                                                       _window{nullptr},
                                                                                       _renderer{nullptr} {}

    bool init();

    ~RenderContext();

    SDL_Texture* loadTexture(const char* file);

    void setDrawColor(int r, int g, int b);

    void clearScreen();

    void renderSprite(Sprite sprite, rect* dst_rect);

    void present();
private:
    const char* _title;
    int _game_width, _game_height;
    int _window_scale;
    SDL_Window* _window;
    SDL_Renderer* _renderer;
};

bool RenderContext::init() {
    _window = SDL_CreateWindow(_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _game_width * _window_scale,
                               _game_height * _window_scale,
                               SDL_WINDOW_SHOWN);
    if (_window == nullptr) {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    _renderer = SDL_CreateRenderer(_window, -1, (SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED));
    if (_renderer == nullptr) {
        std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_RenderSetLogicalSize(_renderer, _game_width, _game_height);

    return true;
}

RenderContext::~RenderContext() {
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
}


SDL_Texture* RenderContext::loadTexture(const char* file) {
    return IMG_LoadTexture(_renderer, file);
}

void RenderContext::setDrawColor(int r, int g, int b) {
    SDL_SetRenderDrawColor(_renderer, r, g, b, 0xFF);
}

void RenderContext::clearScreen() {
    SDL_RenderClear(_renderer);
}

void RenderContext::renderSprite(Sprite sprite, rect* dst_rect) {
    SDL_RenderCopy(_renderer, sprite.texture, &sprite.src_rect, dst_rect);
}

void RenderContext::present() {
    SDL_RenderPresent(_renderer);
}


enum Suit {
    Spades = 0,
    Diamonds = 1,
    Clubs = 2,
    Hearts = 3
};

enum CardColor {
    Black,
    Red
};

struct Card {
    Card(SpriteSheet* src_sheet, Suit suit, int rank);

    CardColor getColor();

    Sprite getSprite();

    rect getArea();

    void setPosition(int x, int y);

    int getRank() { return rank; }

    Suit suit;
    int rank;
    Sprite sprite;
    rect area;
};

Card::Card(SpriteSheet* src_sheet, Suit suit, int rank)
        : suit{suit}, rank{rank}, sprite{src_sheet->createSprite(suit, rank - 1)},
          area{0, 0, sprite.src_rect.w, sprite.src_rect.h} {}

CardColor Card::getColor() {
    if (suit == Suit::Spades || suit == Suit::Clubs) {
        return CardColor::Black;
    }

    return CardColor::Red;
}

Sprite Card::getSprite() {
    return sprite;
}

rect Card::getArea() { return area; }

void Card::setPosition(int x, int y) {
    area.x = x;
    area.y = y;
}


int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    } else {
        RenderContext renderContext("Solitaire", SCREEN_WIDTH, SCREEN_HEIGHT, 3);
        if (renderContext.init()) {
            SpriteSheet spriteSheet(renderContext.loadTexture(ASSETS_PATH"card_spritesheet.png"), 32, 48);


            std::vector<Card> cards{};
            cards.reserve(10);
            for (int i = 0; i < 10; ++i) {
                cards.emplace_back(&spriteSheet, Suit::Spades, i + 2);
                cards.back().setPosition(20, 20 + (i * 13));
            }

            bool quit = false;
            SDL_Event e;

            while (!quit) {
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

                    if (e.type == SDL_MOUSEBUTTONUP) {
                        auto x = e.button.x;
                        auto y = e.button.y;
                        std::cout << "X: " << x << "\nY: " << y << std::endl;
                        for (auto card_i = cards.rbegin(); card_i != cards.rend(); ++card_i) {
                            rect c_area{card_i->getArea()};
                            if (x > c_area.x && x < c_area.x + c_area.w && y > c_area.y && y < c_area.y + c_area.h) {
                                std::cout << "Rank: " << card_i->getRank() << "\n" << std::endl;
                                break;
                            }
                        }
                    }
                }

                renderContext.setDrawColor(22, 128, 17);
                renderContext.clearScreen();

                for (auto card_i = cards.begin(); card_i != cards.end(); ++card_i) {
                    renderContext.renderSprite(card_i->getSprite(), &card_i->area);
                }

                renderContext.present();
            }
        }
    }

    SDL_Quit();

    return 0;
}
