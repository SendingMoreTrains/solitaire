class GameScene : BaseScene
{
private:
    SceneManager* manager;
    Solitiare solitaire;

public:
    // void* here is the game to initialize
    virtual void GameScene(SceneManager* manager, BaseGame* game)
        : manager(manager)
        , solitaire(manager->get_render_context(), game)
    {
        solitaire.start_game();
    }

    virtual void update(InputState* input_state)
    {
        solitaire.update(input_state);
    }

    virtual void render(RenderContext* rc)
    {
        solitaire.render(rc);
    }
}
