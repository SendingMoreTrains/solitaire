class CardSprites
{
public:
    CardSprites(SpriteSheet* sprite_sheet)
        : sprite_sheet{ sprite_sheet }
        , sprites{}
    {
        for (int c = 0; c < 4; ++c)
        {
            for (int r = 0; r < 16; ++r)
            {
                sprites[(r * 4) + c] = sprite_sheet->createSprite(c, r);
            }
        }
    }

    int calc_index(int column, int row)
    {
        return (row * 4) + column;
    }

    Sprite get_card_sprite(Suit suit, int rank)
    {
        return sprites[calc_index((int)suit, (rank - 1))];
    }

    Sprite get_black_joker_sprite()
    {
        return sprites[calc_index(0, 13)];
    }

    Sprite get_red_joker_sprite()
    {
        return sprites[calc_index(1, 13)];
    }

    Sprite get_blue_back_sprite()
    {
        return sprites[calc_index(2, 13)];
    }

    Sprite get_red_back_sprite()
    {
        return sprites[calc_index(3, 13)];
    }

    Sprite get_empty_sprite()
    {
        return sprites[calc_index(0, 15)];
    }

    Sprite get_empty_suit_sprite(Suit suit)
    {
        return sprites[calc_index((int)suit, 14)];
    }

private:
    SpriteSheet* sprite_sheet;
    std::array<Sprite, 64> sprites;
};
