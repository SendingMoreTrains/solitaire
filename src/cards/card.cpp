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

struct Card
{
    Suit suit;
    int rank;
    Sprite sprite;
    rect area;

    Card(SpriteSheet* src_sheet, Suit suit, int rank);

    CardColor getColor();

    void setPosition(int x, int y);
    void render(RenderContext* rc);
};

Card::Card(SpriteSheet* src_sheet, Suit suit, int rank)
    : suit{ suit }
    , rank{ rank }
    , sprite{ src_sheet->createSprite(suit, rank - 1) }
    , area{ 0, 0, sprite.src_rect.w, sprite.src_rect.h }
{}

CardColor Card::getColor()
{
    if (suit == Suit::Spades || suit == Suit::Clubs)
    {
        return CardColor::Black;
    }

    return CardColor::Red;
}

void Card::setPosition(int x, int y)
{
    area.x = x;
    area.y = y;
}

void Card::render(RenderContext* rc)
{
    rc->renderSprite(sprite, &area);
}
