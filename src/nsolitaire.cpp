#include <map>
#include <utility>

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

    bool is_within_bounds(vec2 pos)
    {
        return (pos.x > area.x
                && pos.x < area.x + area.w
                && pos.y > area.y
                && pos.y < area.y + area.h);
    }

    CardColor get_color()
    {
        if (suit == Spades || suit == Clubs) { return Black; }
        return Red;
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

        return result;
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

    void shuffle()
    {
        // TODO: Shuffle cards here
    }
};

struct DragState
{
    bool active;
    std::vector<Card> cards;
    vec2 mouse_offset;

    void end_drag()
    {
        active = false;
        cards.clear();
    }
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

    virtual void handle_click() {} // mouse down & up on same element
    virtual void handle_drag_start() {} // mouse goes down and starts to move

    virtual bool can_accept_card(Card* incoming_card) = 0;

    virtual void render(RenderContext* rc)
    {
        rc->renderSprite(sprite, &area);
    }

    // Assumes cards flow either left->right or top->bottom
    bool is_within_bounds(vec2 pos)
    {
        vec2 upper_left{ area.x, area.y };

        rect lower_right_area;
        if (cards.empty()) { lower_right_area = area; }
        else { lower_right_area = cards.back().area; }

        rect effective_area{
            upper_left.x,
            upper_left.y,
            (lower_right_area.x + lower_right_area.w) - upper_left.x,
            (lower_right_area.y + lower_right_area.h) - upper_left.x
        };

        return (pos.x > effective_area.x
                && pos.x < effective_area.x + effective_area.w
                && pos.y > effective_area.y
                && pos.y < effective_area.y + effective_area.h);
    }

    Card* card_clicked(vec2 pos)
    {
        if (cards.empty()) { return nullptr; }

        for(auto card_i = cards.rbegin(); card_i != cards.rend(); ++card_i)
        {
            Card* card = &(*card_i);
            if (card->is_within_bounds(pos)) { return card; }
        }

        return nullptr;
    }

    void take_cards(std::vector<Card>* incoming_cards)
    {
        std::move(incoming_cards->begin(), incoming_cards->end(), std::back_inserter(cards));
        incoming_cards->erase(incoming_cards->begin(), incoming_cards->end());
    }
};


using PileRenderFunction = void(*)(RenderContext* rc, std::vector<Card>* cards);
namespace PileRenderFunctions
{
    void RenderOffsetStack(RenderContext* rc, std::vector<Card>* cards)
    {

    }

    void RenderFan(RenderContext* rc, std::vector<Card>* cards)
    {

    }
}

using PileAcceptFunction = bool(*)(std::vector<Card>* cards, Card* incoming_card);
namespace PileAcceptFunctions
{
    bool AlternateColorsDescendingRank(std::vector<Card>* cards, Card* incoming_card)
    {
        return
            cards->back().get_color() != incoming_card->get_color() // alternates color
            && ((cards->back().rank - incoming_card->rank) == 1);    // rank is one higher
    }
}



template <PileRenderFunction RenderFunction, PileAcceptFunction AcceptFunction>
struct Pile : TableauItem
{
    // mouse down & up on same element
    virtual void handle_click()
    {

    };

    // mouse goes down and starts to move
    virtual void handle_drag_start()
    {

    };

    virtual bool can_accept_card(Card* incoming_card)
    {
        return AcceptFunction(&cards, incoming_card);
    }

    virtual void render(RenderContext* rc)
    {
        if (cards.empty()) { rc->renderSprite(sprite, &area); }
        else { RenderFunction(rc, &cards); }
    }
};

/* Game/Board does following:
 *   Defines TableauItems and initial positions -\
 *   Creates deck with required cards           --+-> initialize_board
 *   Deals cards to TableauItems                -/
 *   Checks for win condition                   ----> is_game_over
 */
struct GameState
{
    DragState drag_state; // Cards being dragged
    std::vector<TableauItem> tableau_items; // Board elemeents like foundations & piles

    //-----------------------
    // Cards themselves
    // Render order needs to be handled by owning tableau items
    // Delegate logic to tableau items?
    // std::vector<Card> cards;
    Deck deck;
    //-----------------------

    GameState(SpriteSheet* sprite_sheet)
        : deck{ sprite_sheet }
    {}
};


struct NewPile
{
    PileRenderFunction prf;
    PileAcceptFunction paf;

    Sprite empty_sprite;

    NewPile(PileRenderFunction prf, PileAcceptFunction paf, Sprite empty_sprite)
        : prf{ prf }
        , paf{ paf }
        , empty_sprite{ empty_sprite }
    {}
};


struct TableauState
{
    std::map<int, std::vector<NewPile>> pile_map;

    std::vector<NewPile>* get_piles_of_type(int pile_type)
    {
        return &pile_map[pile_type];
    }

    // NOTE: this takes ownership of the incoming pile
    void add_pile(int pile_type, NewPile* incoming_pile)
    {
        if (pile_map.count(pile_type) == 0)
        {
            pile_map.emplace(std::make_pair(pile_type, std::vector<NewPile>()));
        }

        pile_map[pile_type].push_back(std::move(*incoming_pile));
    }
};


struct Game
{
    enum PileTypes
    {
        Cascade = 0,
        Foundation = 1
    };

    virtual ~Game() {};

    // Create deck
    // Create and add tableau_items in their location
    // Deal cards from deck to Tableau
    //  Return to Solitaire to let game run
    virtual void initialize_board(GameState* state) = 0;
    virtual void is_game_over() = 0;
};

struct Playground : Game
{
    enum PileTypes
    {
        Cascade = 0
    };

    virtual void initialize_board(GameState* state)
    {
        state->deck.add_deck();
        state->deck.shuffle();
        std::cout << "Size: " << state->deck.cards.size() << std::endl;


    }

    virtual void is_game_over()
    {

    }
};

class Solitaire
{
    SpriteSheet sprite_sheet;
    // Board board;
    GameState state;
    Game* game;

public:
    Solitaire(RenderContext* render_context)
        : sprite_sheet(render_context->loadTexture("res/card_spritesheet.png"), 32, 48)
        , state(&sprite_sheet)
    {
        Playground pg{};
        pg.initialize_board(&state);
    }

    ~Solitaire()
    {
        delete game;
    }

    void update(InputState* input_state)
    {

    }

    void render(RenderContext* render_context)
    {

    }
};
