struct MenuOption
{
    int id;
    RenderedText text;

    MenuOption(int id, RenderedText text)
        : id{id}
        , text{text}
    {}
};

class Menu
{
    std::vector<MenuOption> options;
    vec2 last_mouse_pos;

    RenderedText indicator;
    int active_index;

public:
    Menu(RenderContext* rc)
        : options{}
        , last_mouse_pos{-1, -1}
        , indicator{ rc->create_rendered_text(">") }
        , active_index{-1}
    {}

    ~Menu()
    {
        for (auto& option : options)
        {
            option.text.free();
        }
        options.clear();
    }

    void add_menu_option(RenderContext* rc, int id, const char* text, vec2 pos)
    {
        options.emplace_back(id, rc->create_rendered_text(text));
        options.back().text.set_position(pos);

        if (options.size() == 1)
        {
            set_active_index(0);
        }
    }

    void update_indicator_pos()
    {
        indicator.set_position(options[active_index].text.get_position() - vec2 {10, 0});
    }

    int get_option_index_at_pos(vec2 pos)
    {
        for (auto option = options.begin(); option != options.end(); ++option)
        {
            if (option->text.is_within_bounds(pos))
            {
                return (int) std::distance(options.begin(), option);
            }
        }

        return -1;
    }

    int next_option_index()
    {
        int next_index{ active_index + 1 };

        if (next_index == (int)options.size())
        {
            return 0;
        }

        return next_index;
    }

    int prev_option_index()
    {
        if (active_index == 0)
        {
            return (int)options.size() - 1;
        }

        return active_index - 1;
    }

    void set_active_index(int index)
    {
        active_index = index;
        update_indicator_pos();
     }

    int update(InputState* input_state)
    {
        if (last_mouse_pos == vec2 {-1, -1})
        {
            last_mouse_pos = input_state->mouse.pos;
        }


        // MOUSE UPDATE CODE
        int index_under_mouse{ get_option_index_at_pos(input_state->mouse.pos) };

        if (index_under_mouse >= 0)
        {
            if (input_state->mouse.left.was_pressed)
            {
                return options[index_under_mouse].id;
            }

            if (last_mouse_pos != input_state->mouse.pos)
            {
                last_mouse_pos = input_state->mouse.pos;
                set_active_index(index_under_mouse);
            }
        }

        // KEYBOARD UPDATE CODE
        if (input_state->keys.get_state(SDLK_DOWN).was_pressed)
        {
            set_active_index(next_option_index());
        }
        else if (input_state->keys.get_state(SDLK_UP).was_pressed)
        {
            set_active_index(prev_option_index());
        }

        if (input_state->keys.get_state(SDLK_RETURN).was_pressed)
        {
            return options[active_index].id;
        }

        return -1;
    }

    void render(RenderContext* rc)
    {
        for (auto& option : options)
        {
            rc->render_text(option.text);
        }

        if (active_index >= 0)
        {
            rc->render_text(indicator);
        }
    }
};
