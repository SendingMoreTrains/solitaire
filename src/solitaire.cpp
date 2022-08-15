#include "cards/cards.h"

struct Pile
{
    std::vector<Card> cards;
    Sprite sprite;
    rect area;

    Pile(SpriteSheet* src_sheet)
        : sprite{ src_sheet->createSprite(0, 15) }
        , area{ 0, 0, sprite.src_rect.w, sprite.src_rect.h }
    {}

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

    void render(RenderContext* rc)
    {
        if (cards.empty())
        {
            rc->renderSprite(sprite, &area);
        }
        else
        {
            for (auto& card : cards)
            {
                card.render(rc);
            }
        }
    }
};

Card* scanForClick(Pile* pile, int x, int y)
{
    for (auto card_i = pile->cards.rbegin(); card_i != pile->cards.rend(); ++card_i) {
        rect c_area{card_i->area};
        if (x > c_area.x && x < c_area.x + c_area.w && y > c_area.y && y < c_area.y + c_area.h) {
            return &*card_i;
        }
    }

    return nullptr;
}

class Solitaire
{
    std::vector<Card> cards;
    SpriteSheet spriteSheet;

    Pile pileOne;
    Pile pileTwo;

public:
    Solitaire(RenderContext* renderContext)
        : spriteSheet(renderContext->loadTexture("res/card_spritesheet.png"), 32, 48)
        , pileOne(&spriteSheet)
        , pileTwo(&spriteSheet)
    {
        pileOne.cards.reserve(10);
        pileOne.setPosition(20, 20);

        pileTwo.cards.reserve(10);
        pileTwo.setPosition(100, 20);

        for (int i = 0; i < 10; ++i) {
            pileOne.cards.emplace_back(&spriteSheet, static_cast<Suit>(i % 4), i + 2);
            pileOne.cards.back().setPosition(20, 20 + (i * 13));

            pileTwo.cards.emplace_back(&spriteSheet, static_cast<Suit>((i + 3) % 4), i + 3);
            pileTwo.cards.back().setPosition(100, 20 + (i * 13));
        }
    }

    void update(InputState* inputState)
    {
        if (inputState->mouse.left.was_released) {
            auto x = inputState->mouse.x;
            auto y = inputState->mouse.y;

            Card* clicked = scanForClick(&pileOne, x, y);
            if (clicked == nullptr) { clicked = scanForClick(&pileTwo, x, y); }

            std::cout << "X: " << x << "\nY: " << y << std::endl;
            if (clicked != nullptr)
            {
                std::cout << "Rank: " << clicked->rank << ", Suit: " << clicked->suit << "\n";
            }
            std::cout << std::endl;
        }
    }

    void render(RenderContext* renderContext)
    {
        renderContext->setDrawColor(22, 128, 17);
        renderContext->clearScreen();

        pileOne.render(renderContext);
        pileTwo.render(renderContext);

        renderContext->present();
    }
};
