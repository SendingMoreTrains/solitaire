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
        Deck deck{ card_sprites };
        deck.add_deck();
        deck.shuffle();

        // Initialize Free Cells
        for (int i = 0; i < 4; ++i)
        {

        }

        // Initialize Foundations
        for (int i = 0; i < 4; ++i)
        {
            // auto temp = new Pile(
            //                      PileAcceptFunctions::FollowOrdering);
        }

        // Initialize Cascades
        for (int i = 0; i < 8; ++i)
        {

        }

        // Deal cards
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
