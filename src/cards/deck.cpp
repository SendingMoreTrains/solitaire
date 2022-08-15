using DeckGenerationFunction = std::vector<Card>(*)(CardSprites*);
namespace DeckGenerators
{
    std::vector<Card> create_normal_deck(CardSprites* sprites)
    {
        std::vector<Card> result;
        result.reserve(52);

        for (Suit s : { Suit::Spades, Suit::Diamonds, Suit::Clubs, Suit::Hearts })
        {
            for (int r = 1; r < 14; ++r)
            {
                result.emplace_back(s, r, sprites->get_card_sprite(s, r));
            }
        }

        return result;
    }
}


class Deck
{
private:
    CardSprites* card_sprites;
    std::vector<Card> cards;

public:
    Deck(CardSprites* card_sprites)
        : card_sprites{ card_sprites }
        , cards{}
    {}

    void add_deck(DeckGenerationFunction generator = DeckGenerators::create_normal_deck)
    {
        std::vector<Card> new_cards{ generator(card_sprites) };

        cards.reserve(cards.capacity() + new_cards.size());
        std::move(new_cards.begin(), new_cards.end(), std::back_inserter(cards));
    }

    void add_decks(int num, DeckGenerationFunction generator = DeckGenerators::create_normal_deck)
    {
        for(int i = 0; i < num; ++i)
        {
            add_deck(generator);
        }
    }

    void shuffle()
    {
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        // std::random_device rd{  };
        std::mt19937 rng {seed};
        std::shuffle(cards.begin(), cards.end(), rng);
    }

    Card deal_card()
    {
        Card dealt{ cards.back() };
        cards.pop_back();

        return dealt;
    }

    bool is_empty()
    {
        return cards.empty();
    }
};
