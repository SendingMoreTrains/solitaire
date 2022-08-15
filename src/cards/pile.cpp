struct Pile
{
    int type_id;
    PileAcceptFunction accept_function;
    PileEmptyAcceptFunction empty_accept_function;
    PileOrderingFunction ordering_function;
    PilePositioningFunction positioning_function;

    Sprite empty_sprite;
    rect area;
    std::vector<Card> cards;

    Pile(int type_id, PileAcceptFunction paf, PileEmptyAcceptFunction peaf, PileOrderingFunction pof, PilePositioningFunction ppf, Sprite empty_sprite, vec2 pos = { 0, 0 })
        : type_id{ type_id }
        , accept_function{ paf }
        , empty_accept_function{ peaf }
        , ordering_function{ pof }
        , positioning_function{ ppf }
        , empty_sprite{ empty_sprite }
        , area{ pos.x, pos.y, empty_sprite.src_rect.w, empty_sprite.src_rect.h }
        , cards{}
    {}

    vec2 get_position() { return vec2 { area.x, area.y }; }

    std::vector<Card>::iterator get_iterator_from_card(Card* card)
    {
        return std::find_if(cards.begin(), cards.end(), [card] (Card const& c) { return &c == card; });
    }

    int get_card_index(Card* card)
    {
        return (int)std::distance(cards.begin(), get_iterator_from_card(card));
    }

    bool can_accept_cards(std::vector<Card>* incoming_cards) {
        return cards.empty() ? empty_accept_function(incoming_cards) : accept_function(&cards, incoming_cards, ordering_function);
    }

    void reset_positions() { positioning_function(get_position(), cards); }

    bool is_card_available(Card* card)
    {
        int card_index = get_card_index(card);

        if (cards.size() == 1 || ((int)cards.size() - 1 == card_index))
        {
            return true;
        }

        int next_card_index = card_index + 1;
        while (next_card_index != (int)cards.size())
        {
            if (!ordering_function(&cards[card_index++], &cards[next_card_index++]))
            {
                return false;
            }
        }

        return true;
    }

    void add_card(Card incoming_card)
    {
        cards.push_back(incoming_card);
        reset_positions();
    }

    bool is_within_bounds(vec2 pos)
    {
        rect lower_bound{ area };
        if (!cards.empty())
        {
            lower_bound = cards.back().area;
        }

        rect effective_area{
            area.x,
            area.y,
            (lower_bound.x + lower_bound.w) - area.x,
            (lower_bound.y + lower_bound.h) - area.y
        };

        return (pos.x > effective_area.x
                && pos.x < effective_area.x + effective_area.w
                && pos.y > effective_area.y
                && pos.y < effective_area.y + effective_area.h);
    }

    std::vector<Card> take_cards(Card* take_from)
    {
        auto card_index = get_iterator_from_card(take_from);

        std::vector<Card> result{};
        while (card_index != cards.end())
        {
            result.push_back(*card_index);
            card_index = cards.erase(card_index);
        }

        return result;
    }

    void receive_cards(std::vector<Card>* incoming_cards)
    {
        std::move(incoming_cards->begin(), incoming_cards->end(), std::back_inserter(cards));
        incoming_cards->clear();

        reset_positions();
    }

    Card* card_clicked(vec2 mouse_pos)
    {
        for (auto it = cards.rbegin(); it != cards.rend(); ++it)
        {
            if (it->is_within_bounds(mouse_pos))
            {
                return &(*it);
            }
        }

        return nullptr;
    }

    void render(RenderContext* rc)
    {
        if (cards.empty())
        {
            rc->renderSprite(empty_sprite, &area);
        }
        else
        {
            for (auto it = cards.begin(); it != cards.end(); ++it)
            {
                it->render(rc);
            }
        }
    }
};
