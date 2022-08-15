#include <map>
#include <utility>
#include <array>
#include <random>
#include <chrono>

#include "cards/cards.h"
#include "games/games.h"


class Solitaire
{
    SpriteSheet sprite_sheet;
    CardSprites card_sprites;
    // Board board;
    GameState state;
    BaseGame* game;
    bool game_won;

public:
    Solitaire(RenderContext* render_context, BaseGame* game)
        : sprite_sheet(render_context->loadTexture("res/card_spritesheet.png"), 32, 48)
        , card_sprites(&sprite_sheet)
        , state()
        , game(game)
        , game_won{ false }
    {}
    ~Solitaire() { delete game; }

    BaseGame* get_game()
    {
        return game;
    }

    void start_game()
    {
        game->initialize_board(&card_sprites, &state);
    }

    void redeal()
    {
        state.clear();
        game->initialize_board(&card_sprites, &state);
    }

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
        if (!state.drag.active) { return; }

        for (auto pile : state.tableau.all_piles)
        {
            if (pile->is_within_bounds(mouse_pos) && pile->can_accept_cards(&state.drag.cards) && game->allow_drop(&state, &(*pile)))
            {
                pile->receive_cards(&state.drag.cards);
            }
        }

        state.drag.end_drag();
    }

    // Useful for ending drag from scene
    void end_drag()
    {
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
