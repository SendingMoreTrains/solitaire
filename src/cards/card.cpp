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

    Card(Suit suit, int rank, Sprite sprite)
        : suit{ suit }
        , rank{ rank }
        , sprite{ sprite }
        , area{ sprite.src_rect }
    {}

    bool is_within_bounds(vec2 pos)
    {
        return (pos.x > area.x
                && pos.x < area.x + area.w
                && pos.y > area.y
                && pos.y < area.y + area.h);
    }

    CardColor get_color()
    {
        if (suit == Spades || suit == Clubs) { return Black; }
        return Red;
    }

    vec2 get_position() { return vec2 { area.x, area.y }; }

    void set_position(vec2 pos)
    {
        area.x = pos.x;
        area.y = pos.y;
    }

    void print()
    {
        std::cout << "Suit: " << (int)suit << ", Rank: " << rank << std::endl;
    }

    void render(RenderContext* rc)
    {
        rc->renderSprite(sprite, &area);
    }
};
