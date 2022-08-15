struct DragState
{
    bool active;
    Card* grabbed_card;
    vec2 mouse_offset;
};

struct TableauItem
{
    rect area;
    Sprite sprite;

    TableauItem(rect area, Sprite sprite)
        : area{ area }
        , sprite{ sprite }
    {}

    virtual void handle_click() {}; // mouse down & up on same element
    virtual void handle_drag_start() {}; // mouse goes down and starts to move
    virtual void handle_drop(DragState* dragState) {}; // mouse released from drag

    bool is_within_bounds(vec2 pos)
    {
        return (
            area.x > r->x
            && area.x < r->x + r->w
            && area.y > r->y
            && area.y < r->y + r->h);
    }

    void render(const RenderContext& rc)
    {
        rc.renderSprite(sprite, &area);
    }
};

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

struct Card : TableauItem
{
    Suit suit;
    int rank;
    Sprite sprite;
};

struct Deck
{
    std::vector<Card> cards;
};

struct GameState
{
    DragState dragState; // Cards being dragged
    std::vector<TableauItem> tableauItems; // Board elemeents like foundations & piles

    //-----------------------
    // Cards themselves
    // Render order needs to be handled by owning tableau items
    // Delegate logic to tableau items?
    std::vector<Card> cards;
    Deck cards;
    //-----------------------
};

struct Card
{
    Suit suit;
    int rank;
    Sprite sprite;
    rect area;

    TableauItem* owning_item;

    void handle_drop(DragState& dragState)
    {
        owning_item->handle_drop(dragState);
    }
};

TableauItem::handle_drop(DragState& dragState)
{
    if (can_accept(cards.back(), dragState.cards.front()))
    {
        take_cards(&dragState.cards);
        dragState.end_drag();
    }
}

// Game/Board does following:
//  Defines TableauItems and initial positions -\
//  Creates deck with required cards           --+-> initialize_board
//  Deals cards to TableauItems                -/
//  Checks for win condition                   ----> is_game_over

struct GameState
{
    DragState drag_state;
    std::vector<TableauItem> tableau_items;
    Deck deck;
};

struct Game
{
    // Create deck
    // Create and add tableau_items in their location
    // Deal cards from deck to Tableau
    //  Return to Solitaire to let game run
    virtual void initialize_board(GameState* state) = 0;
    virtual void is_game_over() = 0;
};

struct Solitaire
{
    // Board board;
    GameState state;
};
