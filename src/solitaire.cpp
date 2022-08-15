#include "cards/cards.h"

bool withinBounds(rect* r, vec2 pos)
{
    return (
        pos.x > r->x
        && pos.x < r->x + r->w
        && pos.y > r->y
        && pos.y < r->y + r->h);
}

struct Pile
{
    std::vector<Card> cards;
    Sprite sprite;
    rect area;

    Pile()
        : sprite{  }
        , area{ 0, 0, sprite.src_rect.w, sprite.src_rect.h }
    {}


    Pile(Sprite sprite)
        : sprite{ sprite }
        , area{ 0, 0, sprite.src_rect.w, sprite.src_rect.h }
    {}

    void addCard(Card incoming)
    {
        vec2 next_card_pos{ area.x, area.y };

        if (!cards.empty())
        {
            next_card_pos.y = cards.back().getPosition().y + 13;
        }

        incoming.setPosition(next_card_pos);
        cards.push_back(incoming);
    }

    void addCards(std::vector<Card> incoming_cards)
    {
        cards.reserve(cards.size() + incoming_cards.size());
        cards.insert(cards.end(), incoming_cards.begin(), incoming_cards.end());
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

    vec2 getPosition()
    {
        return vec2{ area.x, area.y };
    }

    std::vector<Card> takeFromCard(Card* card)
    {
        auto index = cards.begin() + std::distance(&cards[0], card);

        std::vector<Card> new_cards{};
        new_cards.reserve(std::distance(index, cards.end()) + 6);

        while (index != cards.end())
        {
            new_cards.push_back(*index);
            index = cards.erase(index);
        }

        return new_cards;
    }

    void render(RenderContext* rc)
    {
        if (cards.empty() && sprite.texture != nullptr)
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
        if (withinBounds(&card_i->area, vec2{ x, y }))
        {
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

    bool dragging;
    Pile dragPile;
    vec2 dragOffset;

public:
    Solitaire(RenderContext* renderContext)
        : spriteSheet(renderContext->loadTexture("res/card_spritesheet.png"), 32, 48)
        , pileOne(spriteSheet.createSprite(0, 15))
        , pileTwo(spriteSheet.createSprite(0, 15))
        , dragging{ false }
        , dragOffset{}
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
        auto mouse_x = inputState->mouse.x;
        auto mouse_y = inputState->mouse.y;
        if (inputState->mouse.left.was_pressed)
        {
            Card* clicked_card = nullptr;
            clicked_card = scanForClick(&pileOne, mouse_x, mouse_y);
            if (clicked_card)
            {
                vec2 clicked_card_pos{ clicked_card->getPosition() };
                dragPile.addCards(pileOne.takeFromCard(clicked_card));
                dragOffset = vec2{ mouse_x - clicked_card_pos.x, mouse_y - clicked_card_pos.y };
                dragging = true;
            }

            clicked_card = scanForClick(&pileTwo, mouse_x, mouse_y);
            if (clicked_card)
            {
                vec2 clicked_card_pos{ clicked_card->getPosition() };
                dragPile.addCards(pileTwo.takeFromCard(clicked_card));
                dragOffset = vec2{ mouse_x - clicked_card_pos.x, mouse_y - clicked_card_pos.y };
                dragging = true;
            }
        }

        if (dragging)
        {
            dragPile.setPosition(mouse_x - dragOffset.x, mouse_y - dragOffset.y);
        }

        if (inputState->mouse.left.was_released)
        {
            dragging = false;
            if (dragPile.cards.size() > 0)
            {
                dragPile.cards.clear();
            }
        }
    }

    void render(RenderContext* renderContext)
    {
        renderContext->setDrawColor(22, 128, 17);
        renderContext->clearScreen();

        pileOne.render(renderContext);
        pileTwo.render(renderContext);

        if (dragging)
        {
            dragPile.render(renderContext);
        }

        renderContext->present();
    }
};
