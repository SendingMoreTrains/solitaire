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
            .set_ordering_function(PileOrderingFunctions::Any)
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
        for (int i = 0; i < 8; ++i)
        {

        }
        return;

        // Deal cards
        Deck deck{ card_sprites };
        deck.add_deck();
        deck.shuffle();

        auto cascades{ state->tableau.get_piles_of_type(Cascade) };
        while (!deck.is_empty())
        {

            deck.deal_card();
        }
    }

    virtual bool is_game_won(GameState* state)
    {
        for (auto pile : state->tableau.get_piles_of_type(Foundation))
        {
            if (pile->cards.size() < 13)
            {
                return false;
            }
        }

        return true;
    }
};
