#include <map>
#include <utility>
#include <array>
#include <random>
#include <chrono>
#include <cmath>

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

    Card(Suit suit, int rank, Sprite sprite)
        : suit{ suit }
        , rank{ rank }
        , sprite{ sprite }
        , area{ sprite.src_rect }
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

    vec2 get_position() { return vec2 { area.x, area.y }; }

    void set_position(vec2 pos)
    {
        area.x = pos.x;
        area.y = pos.y;
    }

    void print()
    {
        std::cout << "Suit: " << (int)suit << ", Rank: " << rank << std::endl;
    }

    void render(RenderContext* rc)
    {
        rc->renderSprite(sprite, &area);
    }
};

class CardSprites
{
public:
    CardSprites(SpriteSheet* sprite_sheet)
        : sprite_sheet{ sprite_sheet }
        , sprites{}
    {
        for (int c = 0; c < 4; ++c)
        {
            for (int r = 0; r < 16; ++r)
            {
                sprites[(r * 4) + c] = sprite_sheet->createSprite(c, r);
            }
        }
    }

    int calc_index(int column, int row)
    {
        return (row * 4) + column;
    }

    Sprite get_card_sprite(Suit suit, int rank)
    {
        return sprites[calc_index((int)suit, (rank - 1))];
    }

    Sprite get_black_joker_sprite()
    {
        return sprites[calc_index(0, 13)];
    }

    Sprite get_red_joker_sprite()
    {
        return sprites[calc_index(1, 13)];
    }

    Sprite get_blue_back_sprite()
    {
        return sprites[calc_index(2, 13)];
    }

    Sprite get_red_back_sprite()
    {
        return sprites[calc_index(3, 13)];
    }

    Sprite get_empty_sprite()
    {
        return sprites[calc_index(0, 15)];
    }

    Sprite get_empty_suit_sprite(Suit suit)
    {
        return sprites[calc_index((int)suit, 14)];
    }

private:
    SpriteSheet* sprite_sheet;
    std::array<Sprite, 64> sprites;
};


using DeckGenerationFunction = std::vector<Card>(*)(CardSprites*);
namespace DeckGenerators
{
    std::vector<Card> create_normal_deck(CardSprites* sprites)
    {
        std::vector<Card> result;
        result.reserve(52);

        for (Suit s : { Suit::Spades, Suit::Diamonds, Suit::Clubs, Suit::Hearts })
        {
            for (int r = 1; r < 14; ++r)
            {
                result.emplace_back(s, r, sprites->get_card_sprite(s, r));
            }
        }

        return result;
    }
}

class Deck
{
private:
    CardSprites* card_sprites;
    std::vector<Card> cards;

public:
    Deck(CardSprites* card_sprites)
        : card_sprites{ card_sprites }
        , cards{}
    {}

    void add_deck(DeckGenerationFunction generator = DeckGenerators::create_normal_deck)
    {
        std::vector<Card> new_cards{ generator(card_sprites) };

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
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        // std::random_device rd{  };
        std::mt19937 rng {seed};
        std::shuffle(cards.begin(), cards.end(), rng);
    }

    Card deal_card()
    {
        Card dealt{ cards.back() };
        cards.pop_back();

        return dealt;
    }

    bool is_empty()
    {
        return cards.empty();
    }
};


#include "cards/pile_strategies.h"


struct Pile
{
    PileAcceptFunction accept_function;
    PileEmptyAcceptFunction empty_accept_function;
    PileOrderingFunction ordering_function;
    PilePositioningFunction positioning_function;

    Sprite empty_sprite;
    rect area;
    std::vector<Card> cards;

    Pile(PileAcceptFunction paf, PileEmptyAcceptFunction peaf, PileOrderingFunction pof, PilePositioningFunction ppf, Sprite empty_sprite, vec2 pos = { 0, 0 })
        : accept_function{ paf }
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


class PileBuilder
{
private:
    PileAcceptFunction accept_function;
    PileEmptyAcceptFunction empty_accept_function;
    PileOrderingFunction ordering_function;
    PilePositioningFunction positioning_function;
    Sprite empty_sprite;
    vec2 pile_pos;

public:
    void reset()
    {
        accept_function = PileAcceptFunctions::Any;
        empty_accept_function = PileEmptyAcceptFunctions::Any;
        ordering_function = PileOrderingFunctions::Any;
        positioning_function = PilePositioningFunctions::OffsetCascade;
        empty_sprite = Sprite();
        pile_pos = vec2 { 0, 0 };
    }

    PileBuilder()
    {
        reset();
    }

    Pile* build()
    {
        return new Pile(accept_function, empty_accept_function, ordering_function, positioning_function, empty_sprite, pile_pos);
    }

    PileBuilder& set_accept_function(PileAcceptFunction paf)
    {
        accept_function = paf;
        return *this;
    }

    PileBuilder& set_empty_accept_function(PileEmptyAcceptFunction peaf)
    {
        empty_accept_function = peaf;
        return *this;
    }

    PileBuilder& set_ordering_function(PileOrderingFunction pof)
    {
        ordering_function = pof;
        return *this;
    }

    PileBuilder& set_positioning_function(PilePositioningFunction ppf)
    {
        positioning_function = ppf;
        return *this;
    }

    PileBuilder& set_empty_sprite(Sprite sprite)
    {
        empty_sprite = sprite;
        return *this;
    }

    PileBuilder& set_pile_pos(vec2 pos)
    {
        pile_pos = pos;
        return *this;
    }

};


struct DragState
{
    PilePositioningFunction positioning_function;
    std::vector<Card> cards;
    bool active;
    vec2 mouse_offset;
    Pile* source_pile;

    void start_drag(vec2 mouse_pos, Pile* pile, Card* top_card)
    {
        cards = pile->take_cards(top_card);
        mouse_offset = cards.front().get_position() - mouse_pos;
        positioning_function = pile->positioning_function;
        source_pile = pile;
        active = true;
    }

    void update(vec2 mouse_pos)
    {
        if (!active) { return; }

        positioning_function(mouse_pos + mouse_offset, cards);
    }

    void return_cards()
    {
        source_pile->receive_cards(&cards);
    }

    void end_drag()
    {
        if (!cards.empty()) { return_cards(); }
        active = false;
    }

    void render(RenderContext* rc)
    {
        if (!active) { return; }

        for (auto card : cards)
        {
            card.render(rc);
        }
    }
};

struct TableauState
{
    std::vector<Pile*> all_piles;
    std::map<int, std::vector<Pile*>> pile_map;

    ~TableauState()
    {
        for (auto pile : all_piles)
        {
            delete pile;
        }
    }

    void add_pile(int pile_type, Pile* incoming_pile)
    {
        all_piles.push_back(incoming_pile);
        pile_map[pile_type].push_back(incoming_pile);
    }

    std::vector<Pile*> get_piles_of_type(int pile_type)
    {
        return pile_map[pile_type];
    }
};



using UIFunction = void(*)(TableauState*);
struct UIElement
{
    Sprite sprite;
    rect area;
    UIFunction proc;

    UIElement(Sprite sprite, rect area, UIFunction proc)
        : sprite{ sprite }
        , area{ area }
        , proc{ proc }
    {}

    void handle_click(TableauState* tableau_state)
    {
        proc(tableau_state);
    }

    void render(RenderContext* rc)
    {
        rc->renderSprite(sprite, &area);
    }
};

struct GameState
{
    DragState drag;
    TableauState tableau;
    std::vector<UIElement> ui_elements;
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
    virtual void initialize_board(CardSprites*, GameState*) = 0;
    virtual bool is_game_won(GameState*) = 0;

    // Allow Game to overrule certain actions before starting
    //  e.g. FreeCell limits drag by open cells
    virtual bool allow_drag(GameState*) { return true; }
    virtual bool allow_drop(GameState*, Pile*) { return true; }
};


#include "games/free_cell.cpp"


struct Playground : Game
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
            PileAcceptFunctions::Any,
            PileEmptyAcceptFunctions::Any,
            PileOrderingFunctions::Any,
            PilePositioningFunctions::OffsetCascade,
            sprites->get_empty_sprite(),
            vec2 { 10, 10 });

        Pile* pile_two = new Pile(
            PileAcceptFunctions::Any,
            PileEmptyAcceptFunctions::Any,
            PileOrderingFunctions::Any,
            PilePositioningFunctions::OffsetCascade,
            sprites->get_empty_sprite(),
            vec2 { 50, 10 });

        Pile* foundation_pile = new Pile(
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


class Solitaire
{
    SpriteSheet sprite_sheet;
    CardSprites card_sprites;
    // Board board;
    GameState state;
    Game* game;
    bool game_won;

public:
    Solitaire(RenderContext* render_context)
        : sprite_sheet(render_context->loadTexture("res/card_spritesheet.png"), 32, 48)
        , card_sprites(&sprite_sheet)
        , state()
        , game_won{ false }
    {
        // game = new Playground();
        game = new FreeCell();

        game->initialize_board(&card_sprites, &state);
    }
    ~Solitaire() { delete game; }

    void handle_mouse_press(vec2 mouse_pos)
    {
        Card* clicked_card{};

        for (auto pile : state.tableau.all_piles)
        {
            clicked_card = pile->card_clicked(mouse_pos);

            if (clicked_card && pile->is_card_available(clicked_card))
            {
                state.drag.start_drag(mouse_pos, pile, clicked_card);
                if (!game->allow_drag(&state))
                {
                    state.drag.end_drag();
                }

                break;
            }
        }
    }

    // TODO: Handle drop by overlap instead of mouse_pos
    void handle_mouse_release(vec2 mouse_pos)
    {
        for (auto pile : state.tableau.all_piles)
        {
            if (pile->is_within_bounds(mouse_pos) && pile->can_accept_cards(&state.drag.cards) && game->allow_drop(&state, &(*pile)))
            {
                pile->receive_cards(&state.drag.cards);
            }
        }

        state.drag.end_drag();
    }

    void update(InputState* input_state)
    {
        if (game_won)
        {
            return;
        }

        vec2 mouse_pos{ input_state->mouse.pos };

        if (input_state->mouse.left.was_pressed)
        {
            handle_mouse_press(mouse_pos);
        }

        if (input_state->mouse.left.was_released)
        {
            handle_mouse_release(mouse_pos);

            // Only check for victory after a drag has ended (the board has updated)
            game_won = game->is_game_won(&state);
            if (game_won)
            {
                std::cout << "Game won!" << std::endl;
            }
        }

        // Update at the end. If the drag is over, no need to update
        state.drag.update(input_state->mouse.pos);

    }

    void render(RenderContext* rc)
    {
        rc->setDrawColor(22, 128, 17);
        rc->clearScreen();

        for (auto pile : state.tableau.all_piles)
        {
            pile->render(rc);
        }

        state.drag.render(rc);
    }
};
