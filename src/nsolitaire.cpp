#include <map>
#include <utility>
#include <array>
#include <random>
#include <chrono>

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
    rect area;

    Card(Suit suit, int rank, rect area)
        : suit{ suit }
        , rank{ rank }
        , area{ area }
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
};


using DeckGenerationFunction = std::vector<Card>(*)(rect);
namespace DeckGenerators
{
    std::vector<Card> create_normal_deck(rect card_size)
    {
        std::vector<Card> result;
        result.reserve(52);

        for (Suit s : { Suit::Spades, Suit::Diamonds, Suit::Clubs, Suit::Hearts })
        {
            for (int r = 1; r < 14; ++r)
            {
                result.emplace_back(s, r, card_size);
            }
        }

        return result;
    }
}

struct Deck
{
    std::vector<Card> cards;
    rect card_size;

    Deck(rect card_size)
        : cards{}
        , card_size{ card_size }
    {}

    void add_deck(DeckGenerationFunction generator = DeckGenerators::create_normal_deck)
    {
        std::vector<Card> new_cards{ generator(card_size) };

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
};


// TODO: This is shit, find a better way
class CardRenderer
{
public:
    enum BackColor
    {
        Blue,
        Red
    };

    CardRenderer(SpriteSheet* sprite_sheet, BackColor back_color = BackColor::Blue)
        : sprite_sheet{ sprite_sheet }
        , sprites{}
        , back_color{ back_color }
    {
        for (int s = 0; s < 4; ++s)
        {
            for (int r = 0; r < 14; ++r)
            {
                sprites[(r * 4) + s] = sprite_sheet->createSprite(s, r);
            }
        }
    }

    int calc_index(int column, int row)
    {
        return (row * 4) + column;
    }

    void render_card(RenderContext* rc, Card& card)
    {
        rc->renderSprite(sprites[calc_index((int)card.suit, card.rank-1)], &card.area);
    }

    void render_blue_back(RenderContext* rc, vec2 pos)
    {
        rc->renderSprite(sprites[calc_index(2, 13)], pos);
    }

    void render_red_back(RenderContext* rc, vec2 pos)
    {
        rc->renderSprite(sprites[calc_index(3, 13)], pos);
    }

    void render_back(RenderContext* rc, vec2 pos)
    {
        if (back_color == BackColor::Blue)
        {
            render_blue_back(rc, pos);
        }
        else
        {
            render_red_back(rc, pos);
        }
    }

private:
    SpriteSheet* sprite_sheet;
    std::array<Sprite, 56> sprites;

    BackColor back_color;
};

using PilePositioningFunction = void(*)(vec2, std::vector<Card>&);
namespace PilePositioningFunctions
{
    const int CARD_OFFSET = 13;

    void OffsetCascade(vec2 pile_pos, std::vector<Card>& cards)
    {
        if (cards.empty()) { return; }

        for (int i = 0; i < (int)cards.size(); ++i)
        {
            cards[i].set_position(vec2 { pile_pos.x, pile_pos.y + (i * CARD_OFFSET) });
        }
    }
}


using PileAcceptFunction = bool(*)(std::vector<Card>*, std::vector<Card>*);
namespace PileAcceptFunctions
{
    bool Any(std::vector<Card>* cards, std::vector<Card>* incoming_cards) { return true; }

    bool AnySingle(std::vector<Card>* cards, std::vector<Card>* incoming_cards)
    {
        return incoming_cards->size() == 1;
    }

    bool AlternateColorsDescendingRank(std::vector<Card>* cards, std::vector<Card>* incoming_cards)
    {
        return
            cards->back().get_color() != incoming_cards->front().get_color() // alternates color
            && ((cards->back().rank - incoming_cards->front().rank) == 1);    // rank is one higher
    }
}


using PileOrderingFunction = bool(*)(Card*, Card*);
namespace PileOrderingFunctions
{
    bool Any(Card*, Card*) { return true; }

    bool AlternateColor(Card* top_card, Card* bottom_card)
    {
        return top_card->get_color() != bottom_card->get_color();
    }
}


struct Pile
{
    PilePositioningFunction ppf;
    PileAcceptFunction paf;

    PileOrderingFunction pof;

    Sprite empty_sprite;
    rect area;
    std::vector<Card> cards;

    Pile(PilePositioningFunction ppf, PileAcceptFunction paf, PileOrderingFunction pof, Sprite empty_sprite, vec2 pos = { 0, 0 })
        : ppf{ ppf }
        , paf{ paf }
        , pof{ pof }
        , empty_sprite{ empty_sprite }
        , area{ pos.x, pos.y, empty_sprite.src_rect.w, empty_sprite.src_rect.h }
        , cards{}
    {}

    vec2 get_position() { return vec2 { area.x, area.y }; }

    bool can_accept_card(std::vector<Card>* incoming_cards) { return paf(&cards, incoming_cards); }

    void reset_positions() { ppf(get_position(), cards); }

    bool is_card_available(Card* card)
    {
        // TODO: Implement this using Ordering Function
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
        auto card_index = std::find_if(cards.begin(), cards.end(), [take_from] (Card const& c) { return &c == take_from; });

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

    void render(RenderContext* rc, CardRenderer* cr)
    {
        if (cards.empty())
        {
            rc->renderSprite(empty_sprite, &area);
        }
        else
        {
            for (auto it = cards.begin(); it != cards.end(); ++it)
            {
                cr->render_card(rc, *it);

                // it->render(rc);
            }
        }
    }
};

struct DragState
{
    PilePositioningFunction ppf;
    std::vector<Card> cards;
    bool active;
    vec2 mouse_offset;
    Pile* source_pile;

    void start_drag(vec2 mouse_pos, Pile* pile, Card* top_card)
    {
        cards = pile->take_cards(top_card);
        mouse_offset = cards.front().get_position() - mouse_pos;
        ppf = pile->ppf;
        source_pile = pile;
        active = true;
    }

    void update(vec2 mouse_pos)
    {
        ppf(mouse_pos + mouse_offset, cards);
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

    void render(RenderContext* rc, CardRenderer* cr)
    {
        for (auto card : cards)
        {
            cr->render_card(rc, card);
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
    virtual void initialize_board(SpriteSheet* sprite_sheet, Deck* deck, GameState* state) = 0;

    // Allow Game to overrule drag before starting
    //  e.g. FreeCell limits movement by open cells
    virtual bool allow_drag(/* TODO: Args */) { return true; }


};

struct Playground : Game
{
    enum PileTypes
    {
        Cascade = 0
    };

    virtual void initialize_board(SpriteSheet* sprite_sheet, Deck* deck, GameState* state)
    {
        deck->add_deck();
        deck->shuffle();

        Pile* pile = new Pile(
            PilePositioningFunctions::OffsetCascade,
            PileAcceptFunctions::Any,
            PileOrderingFunctions::Any,
            sprite_sheet->createSprite(0, 14),
            vec2 { 10, 10 });

        Pile* pile_two = new Pile(
            PilePositioningFunctions::OffsetCascade,
            PileAcceptFunctions::AnySingle,
            PileOrderingFunctions::Any,
            sprite_sheet->createSprite(1, 14),
            vec2 { 50, 10 });

        pile_two->add_card(deck->deal_card());
        pile->add_card(deck->deal_card());
        pile->add_card(deck->deal_card());

        state->tableau.add_pile(Cascade, pile);
        state->tableau.add_pile(Cascade, pile_two);
    }

    virtual void is_game_over()
    {

    }
};


class Solitaire
{
    SpriteSheet sprite_sheet;
    CardRenderer card_renderer;
    // Board board;
    GameState state;
    Game* game;

public:
    Solitaire(RenderContext* render_context)
        : sprite_sheet(render_context->loadTexture("res/card_spritesheet.png"), 32, 48)
        , card_renderer(&sprite_sheet)
        , state()
    {
        game = new Playground();

        rect card_size{ 0, 0, sprite_sheet.get_sprite_width(), sprite_sheet.get_sprite_height() };
        Deck deck(card_size);

        game->initialize_board(&sprite_sheet, &deck, &state);
    }
    ~Solitaire() { delete game; }

    void update(InputState* input_state)
    {
        // TODO: Clean this up
        if (state.drag.active)
        {
            state.drag.update(input_state->mouse.pos);
        }

        if (input_state->mouse.left.was_pressed)
        {
            Card* clicked_card{};

            for (auto pile : state.tableau.all_piles)
            {
                clicked_card = pile->card_clicked(input_state->mouse.pos);
                if (clicked_card)
                {
                    state.drag.start_drag(input_state->mouse.pos, pile, clicked_card);
                    break;
                }
            }

        }

        if (input_state->mouse.left.was_released)
        {
            for (auto pile : state.tableau.all_piles)
            {
                if (pile->is_within_bounds(input_state->mouse.pos))
                {
                    if (pile->can_accept_card(&state.drag.cards))
                    {
                        pile->receive_cards(&state.drag.cards);
                    }
                }
            }
            state.drag.end_drag();
        }
    }

    void render(RenderContext* rc)
    {
        rc->setDrawColor(22, 128, 17);
        rc->clearScreen();

        for (auto pile : state.tableau.all_piles)
       {
            pile->render(rc, &card_renderer);
        }

        if (state.drag.active)
        {
            state.drag.render(rc, &card_renderer);
        }
    }
};
