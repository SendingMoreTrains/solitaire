#include <map>
#include <utility>
#include <array>

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
    }

    Card deal_card()
    {
        Card dealt{ cards.back() };
        cards.pop_back();

        return dealt;
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


using PileRenderFunction = void(*)(RenderContext* rc, std::vector<Card>* cards);
namespace PileRenderFunctions
{
    void OffsetStack(RenderContext* rc, std::vector<Card>* cards)
    {

    }

    void Fan(RenderContext* rc, std::vector<Card>* cards)
    {

    }
}

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

using PilePlacementFunction = void(*)(vec2, std::vector<Card>*, Card*);
namespace PilePlacementFunctions
{
    void OffsetCascade(vec2 pile_position, std::vector<Card>* cards, Card* incoming_card)
    {
        if (cards->empty())
        {
            incoming_card->set_position(pile_position);
        }
        else
        {
            incoming_card->set_position(cards->back().get_position() + vec2 { 0, 13 });
        }
    }
}

using PileAcceptFunction = bool(*)(std::vector<Card>* cards, Card* incoming_card);
namespace PileAcceptFunctions
{
    bool Any(std::vector<Card>* cards, Card* incoming_card) { return true; }

    bool AlternateColorsDescendingRank(std::vector<Card>* cards, Card* incoming_card)
    {
        return
            cards->back().get_color() != incoming_card->get_color() // alternates color
            && ((cards->back().rank - incoming_card->rank) == 1);    // rank is one higher
    }
}


struct Pile
{
    PilePlacementFunction ppf;
    PileAcceptFunction paf;

    Sprite empty_sprite;
    rect area;
    std::vector<Card> cards;

    Pile(PilePlacementFunction ppf, PileAcceptFunction paf, Sprite empty_sprite, vec2 pos = { 0, 0 })
        : ppf{ ppf }
        , paf{ paf }
        , empty_sprite{ empty_sprite }
        , area{ pos.x, pos.y, empty_sprite.src_rect.w, empty_sprite.src_rect.h }
        , cards{}
    {}

    bool can_accept_card(Card* incoming_card) { return paf(&cards, incoming_card); }

    vec2 get_position()
    {
        return vec2 { area.x, area.y };
    }

    void add_card(Card incoming_card)
    {
        ppf(get_position(), &cards, &incoming_card);
        cards.push_back(incoming_card);
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


struct TableauState
{
    std::vector<Pile*> all_piles;
    std::map<int, std::vector<Pile*>> pile_map;

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
    virtual void is_game_over() = 0;
};

struct Playground : Game
{
    enum PileTypes
    {
        Cascade = 0
    };

    virtual void initialize_board(SpriteSheet* sprite_sheet, Deck* deck, GameState* state)
    {
        // state->deck.add_deck();
        // state->deck.shuffle();
        // std::cout << "Size: " << state->deck.cards.size() << std::endl;

        deck->add_deck();

        Pile* pile = new Pile(
            PilePlacementFunctions::OffsetCascade,
            PileAcceptFunctions::Any,
            sprite_sheet->createSprite(0, 14),
            vec2 { 10, 10 });

        Pile* pile_two = new Pile(
            PilePlacementFunctions::OffsetCascade,
            PileAcceptFunctions::Any,
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
        std::vector<Pile*> piles{ state.tableau.all_piles };
    }

    void render(RenderContext* rc)
    {
        rc->setDrawColor(22, 128, 17);
        rc->clearScreen();

        std::vector<Pile*>* piles = &state.tableau.all_piles;

        for (auto it = piles->begin(); it != piles->end(); ++it)
        {
            Pile* pile = *it;

            pile->render(rc, &card_renderer);
        }

        card_renderer.render_back(rc, vec2 { 30, 50 });
    }
};
