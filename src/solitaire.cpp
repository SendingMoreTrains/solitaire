#include "cards/cards.h"

struct Pile
{
    std::vector<Card> cards;
    rect area;

    void addCard(Card card)
    {
        cards.push_back(card);
    }

    void setPosition(int x, int y)
    {
        area.x = x;
        area.y = y;

        for (int i = 0; i < static_cast<int>(cards.size()); ++i)
        {
            cards[i].setPosition(x, y + (13 * i));
        }
    }
};

class Solitaire {
    std::vector<Card> cards;
    SpriteSheet* spriteSheet;

    Pile pile;

public:
    Solitaire(RenderContext* renderContext)
    {
        spriteSheet = new SpriteSheet(renderContext->loadTexture("res/card_spritesheet.png"), 32, 48);

        pile.cards.reserve(10);
        pile.setPosition(20, 20);

        for (int i = 0; i < 10; ++i) {
            pile.cards.emplace_back(spriteSheet, static_cast<Suit>(i % 4), i + 2);
            pile.cards.back().setPosition(20, 20 + (i * 13));
        }
    }

    void update(InputState* inputState)
    {
        if (inputState->mouse.left.was_released) {
            auto x = inputState->mouse.x;
            auto y = inputState->mouse.y;
            std::cout << "X: " << x << "\nY: " << y << std::endl;
            for (auto card_i = pile.cards.rbegin(); card_i != pile.cards.rend(); ++card_i) {
                rect c_area{card_i->area};
                if (x > c_area.x && x < c_area.x + c_area.w && y > c_area.y && y < c_area.y + c_area.h) {
                    std::cout << "Rank: " << card_i->rank << ", Suit: " << card_i->suit << "\n" << std::endl;
                    break;
                }
            }
        }

        pile.setPosition(inputState->mouse.x, inputState->mouse.y);

    }

    void render(RenderContext* renderContext)
    {
        renderContext->setDrawColor(22, 128, 17);
        renderContext->clearScreen();


        for (auto& card : pile.cards) {
            card.render(renderContext);
        }

        renderContext->present();
    }
};
