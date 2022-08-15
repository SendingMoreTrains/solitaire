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
