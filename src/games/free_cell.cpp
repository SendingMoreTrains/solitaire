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
            .set_type_id(Cell)
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
            .set_type_id(Foundation)
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
            .set_type_id(Cascade)
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

    int get_max_move_amount(GameState* state, Pile* drop_target = nullptr)
    {
        int open_cells{ 0 };
        int open_cascades{ 0 };

        for (auto const& pile : state->tableau.get_piles_of_type(Cell))
        {
            if (pile->cards.size() == 0
                && &(*pile) != state->drag.source_pile
                && &(*pile) != drop_target)
                ++open_cells;
        }

        for (auto const& pile : state->tableau.get_piles_of_type(Cascade))
        {
            if (pile->cards.size() == 0
                && &(*pile) != state->drag.source_pile
                && &(*pile) != drop_target)
                ++open_cascades;
        }

        int result = (1 + open_cells) * ((1 << open_cascades));

        std::cout << "Moveable amount: " << result << std::endl;

        return result;
    }

    virtual bool allow_drag(GameState* state)
    {
        return (int)state->drag.cards.size() <= get_max_move_amount(state);
    }

    virtual bool allow_drop(GameState* state, Pile* target_pile)
    {
        return (int)state->drag.cards.size() <= get_max_move_amount(state, target_pile);
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
