struct DragState
{
    bool active;
    Card* grabbed_card;
    vec2 mouse_offset;
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

struct Card
{
    Suit suit;
    int rank;
    Sprite sprite;
    rect area;

    Card(SpriteSheet* src_sheet, Suit suit, int rank)
        : suit{ suit }
        , rank{ rank }
        , sprite{ src_sheet->createSprite(suit, rank - 1) }
        , area{ 0, 0, sprite.src_rect.w, sprite.src_rect.h }
    {}
};


struct TableauItem
{
    rect area;
    Sprite sprite;
    std::vector<Card> cards;

    TableauItem(rect area, Sprite sprite)
        : area{ area }
        , sprite{ sprite }
        , cards{}
    {}

    virtual void handle_click() {}; // mouse down & up on same element
    virtual void handle_drag_start() {}; // mouse goes down and starts to move
    virtual void handle_drop(DragState* dragState) {}; // mouse released from drag

    // Assumes cards flow either left->right or top->bottom
    bool is_within_bounds(vec2 pos)
    {
        vec2 upper_left{ area.x, area.y };

        rect lower_right_area;
        if (cards.empty()) { lower_right_area = area }
        else { lower_right_area = cards.back().area }

        rect effective_area{
            upper_left.x;
            upper_left.y;
            (lower_right_area.x + lower_right_area.w) - upper_left.x;
            (lower_right_area.y + lower_right_area.h) - upper_left.x;
        };

        return (pos.x > effective_area->x
                && pos.x < effective_area->x + effective_area->w
                && pos.y > effective_area->y
                && pos.y < effective_area->y + effective_area->h);
    }

    void render(const RenderContext& rc)
    {
        rc.renderSprite(sprite, &area);
    }
};

using DeckGenerationFunction = std::vector<Card>(*)(SpriteSheet*);
namespace DeckGenerators
{
    std::vector<Card> create_normal_deck(SpriteSheet* src_sheet)
    {
        std::vector<Card> result;
        result.reserve(52);

        for (Suit s : { Suit::Spades, Suit::Diamonds, Suit::Clubs, Suit::Hearts })
        {
            for (int r = 0; r < 13; ++r)
            {
                result.emplace_back(src_sheet, s, r);
            }
        }
    }
}

struct Deck
{
    std::vector<Card> cards;
    SpriteSheet* sprite_sheet;

    Deck(SpriteSheet* sprite_sheet)
        : cards{}
        , sprite_sheet{ sprite_sheet }
    {}

    void add_deck(DeckGenerationFunction generator = DeckGenerators::create_normal_deck)
    {
        std::vector<Card> new_cards{ generator(sprite_sheet) };

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

    void shuffle(std::vector<Card>* cards)
    {
        // TODO: Shuffle cards here
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
    DragState drag_state; // Cards being dragged
    std::vector<TableauItem> tableau_items; // Board elemeents like foundations & piles

    //-----------------------
    // Cards themselves
    // Render order needs to be handled by owning tableau items
    // Delegate logic to tableau items?
    std::vector<Card> cards;
    Deck cards;
    //-----------------------
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

struct Playground : Game
{
    virtual void initialize_board(GameState* state)
    {
        state->deck.add_deck();
        state->shuffle();
    }
};

struct Solitaire
{
    // Board board;
    GameState state;
};
