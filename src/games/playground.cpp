struct Playground : BaseGame
{
    enum PileTypes
    {
        Cascade = 0,
        Foundation = 1
    };

    virtual void initialize_board(CardSprites* sprites, GameState* state)
    {
        Deck deck{ sprites };
        deck.add_deck();
        deck.shuffle();

        Pile* pile = new Pile(
            0,
            PileAcceptFunctions::Any,
            PileEmptyAcceptFunctions::Any,
            PileOrderingFunctions::Any,
            PilePositioningFunctions::OffsetCascade,
            sprites->get_empty_sprite(),
            vec2 { 10, 10 });

        Pile* pile_two = new Pile(
            0,
            PileAcceptFunctions::Any,
            PileEmptyAcceptFunctions::Any,
            PileOrderingFunctions::Any,
            PilePositioningFunctions::OffsetCascade,
            sprites->get_empty_sprite(),
            vec2 { 50, 10 });

        Pile* foundation_pile = new Pile(
            0,
            PileAcceptFunctions::Any,
            PileEmptyAcceptFunctions::Any,
            PileOrderingFunctions::Any,
            PilePositioningFunctions::OffsetCascade,
            sprites->get_empty_sprite(),
            vec2 { 140, 20 });

        pile_two->add_card(deck.deal_card());
        pile->add_card(deck.deal_card());
        pile->add_card(deck.deal_card());

        pile_two->add_card(deck.deal_card());
        pile->add_card(deck.deal_card());
        pile->add_card(deck.deal_card());

        pile_two->add_card(deck.deal_card());
        pile->add_card(deck.deal_card());
        pile->add_card(deck.deal_card());

        state->tableau.add_pile(Cascade, pile);
        state->tableau.add_pile(Cascade, pile_two);
        state->tableau.add_pile(Foundation, foundation_pile);
    }

    virtual bool is_game_won(GameState* state)
    {
        for (auto pile : state->tableau.get_piles_of_type(Cascade))
        {
            if (pile->cards.size() > 0) { return false; }
        }

        for (auto pile : state->tableau.get_piles_of_type(Foundation))
        {
            int prev_rank{ 999 };
            for (auto it = pile->cards.begin(); it != pile->cards.end(); ++it)
            {
                if (prev_rank < it->rank) { return false; }
                prev_rank = it->rank;
            }
        }

        return true;
    }
};
