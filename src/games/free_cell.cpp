struct FreeCell : Game
{
    enum PileTypes
    {
        Cell = 0,
        Foundation = 1,
        Cascade = 2
    };

    virtual void initialize_board(CardSprites* card_sprites, GameState* state)
    {
        PileBuilder builder{};

        // Initialize Free Cells
        builder
            .set_accept_function(PileAcceptFunctions::None)
            .set_empty_accept_function(PileEmptyAcceptFunctions::AnySingle)
            .set_positioning_function(PilePositioningFunctions::Aligned)
            .set_empty_sprite(card_sprites->get_empty_sprite());
        for (int i = 0; i < 4; ++i)
        {
            builder.set_pile_pos(vec2 { 10 + (i * 40), 10 });
            state->tableau.add_pile(Cell, builder.build());
        }
        builder.reset();

        // Initialize Foundations
        builder
            .set_accept_function(PileAcceptFunctions::FollowOrderingSingle)
            .set_empty_accept_function(PileEmptyAcceptFunctions::Aces)
            .set_ordering_function(PileOrderingFunctions::IncrementingSameSuit)
            .set_positioning_function(PilePositioningFunctions::Aligned);
        for (int i = 0; i < 4; ++i)
        {
            builder
                .set_empty_sprite(card_sprites->get_empty_suit_sprite((Suit)i))
                .set_pile_pos(vec2 { 50 + (i * 40) + (3 * 40), 10 });
            state->tableau.add_pile(Foundation, builder.build());
        }
        builder.reset();

        // Initialize Cascades
        builder
            .set_accept_function(PileAcceptFunctions::FollowOrdering)
            .set_empty_accept_function(PileEmptyAcceptFunctions::Any)
            .set_ordering_function(PileOrderingFunctions::AlternateColorDecrementRank)
            .set_positioning_function(PilePositioningFunctions::OffsetCascade)
            .set_empty_sprite(card_sprites->get_empty_sprite());
        for (int i = 0; i < 8; ++i)
        {
            builder.set_pile_pos(vec2 { 10 + (i * 40), (8 + 48 + 10) });
            state->tableau.add_pile(Cascade, builder.build());
        }
        builder.reset();

        // Deal cards
        Deck deck{ card_sprites };
        deck.add_deck();
        deck.shuffle();

        auto cascades{ state->tableau.get_piles_of_type(Cascade) };
        int target_pile{ 0 };
        while (!deck.is_empty())
        {
            cascades[(target_pile % 8)]->add_card(deck.deal_card());
            ++target_pile;
        }
    }

    int get_max_drag_amount(GameState* state, bool dropping_on_open_cascade = false)
    {
        int open_cells{ 0 };
        int open_cascades{ 0 };

        for (auto const& pile : state->tableau.get_piles_of_type(Cell))
        {
            if (pile->cards.size() == 0)
                ++open_cells;
        }

        for (auto const& pile : state->tableau.get_piles_of_type(Cascade))
        {
            if (pile->cards.size() == 0)
                ++open_cascades;
        }

        if (dropping_on_open_cascade) { --open_cascades; }

        return (1 + open_cells) * (pow(2, open_cascades));
    }

    virtual bool allow_drag(GameState* state)
    {
        return state->drag.cards.size() <= get_max_drag_amount(state);
    }

    virtual bool allow_drop(GameState* state, Pile* target_pile)
    {
        bool is_pile_empty{ target_pile->cards.size() == 0 };
        return state->drag.cards.size() <= get_max_drag_amount(state, is_pile_empty);
    }

    virtual bool is_game_won(GameState* state)
    {
        for (auto const& pile : state->tableau.get_piles_of_type(Foundation))
        {
            if (pile->cards.size() < 13)
            {
                return false;
            }
        }

        return true;
    }
};
