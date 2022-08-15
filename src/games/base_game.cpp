struct BaseGame
{
    enum PileTypes
    {
        Cascade = 0,
        Foundation = 1
    };

    virtual ~BaseGame() {};

    // Create deck
    // Create and add tableau_items in their location
    // Deal cards from deck to Tableau
    //  Return to Solitaire to let game run
    virtual void initialize_board(CardSprites*, GameState*) = 0;
    virtual bool is_game_won(GameState*) = 0;

    // Allow Game to overrule certain actions before starting
    //  e.g. FreeCell limits drag by open cells
    virtual bool allow_drag(GameState*) { return true; }
    virtual bool allow_drop(GameState*, Pile*) { return true; }
};
