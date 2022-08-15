class GameScene : public BaseScene
{
private:
    SceneManager* manager;
    Solitaire solitaire;

public:
    // void* here is the game to initialize
    GameScene(SceneManager* manager, BaseGame* game)
        : manager(manager)
        , solitaire(manager->get_render_context(), game)
    {
        solitaire.start_game();
    }

    virtual void update(InputState* input_state)
    {
        if (input_state->keys.get_state(SDLK_ESCAPE).was_pressed)
        {
            solitaire.end_drag();
            auto pause_scene = new PauseScene(manager, this);
            manager->set_scene(pause_scene);
            return;
        }

        solitaire.update(input_state);
    }

    virtual void render(RenderContext* rc)
    {
        solitaire.render(rc);
    }
};
