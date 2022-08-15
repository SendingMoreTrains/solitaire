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

    Pile(Sprite sprite)
        : sprite{ sprite }
        , area{ 0, 0, sprite.src_rect.w, sprite.src_rect.h }
    {}

    void addCard(Card card)
    {
        vec2 next_card_pos{ area.x, area.y };

        if (!cards.empty())
        {
            next_card_pos.y = cards.back().getPosition().y + 13;
        }

        card.setPosition(next_card_pos);
        cards.push_back(card);
    }

    void updateCardsPosition()
    {
        for (int i = 0; i < static_cast<int>(cards.size()); ++i)
        {
            cards[i].setPosition(area.x, area.y + (13 * i));
        }
    }

    void setPosition(int x, int y)
    {
        area.x = x;
        area.y = y;

        updateCardsPosition();
    }

    void setPosition(vec2 new_pos)
    {
        area.x = new_pos.x;
        area.y = new_pos.y;

        updateCardsPosition();
    }

    vec2 getPosition() { return vec2{ area.x, area.y }; }

    void takeFromCard(Card* card)
    {
        auto index = cards.begin() + std::distance(&cards[0], card);
        vec2 start_pos{ this->getPosition() };

        // Pile new_pile(sprite);
        // new_pile.setPosition(start_pos);
        while (index != cards.end())
        {
            // new_pile.addCard(*index);
            // index = cards.erase(index);
            std::cout << "Rank: " << index->rank << ", Suit: " << index->suit << "\n";
            ++index;
        }

        // return new_pile;
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
            if (clicked)
            {
                pileOne.takeFromCard(clicked);
            }
            else
            {
                clicked = scanForClick(&pileTwo, x, y);
                if (clicked)
                {
                    pileTwo.takeFromCard(clicked);
                }
            }


            // std::cout << "X: " << x << "\nY: " << y << std::endl;
            // if (clicked != nullptr)
            // {
            //     std::cout << "Rank: " << clicked->rank << ", Suit: " << clicked->suit << "\n";
            // }
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
