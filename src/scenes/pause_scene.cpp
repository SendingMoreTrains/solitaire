class PauseScene : public BaseScene
{
public:
    SceneManager* manager;
    BaseScene* paused_scene;

    SDL_Texture* overlay;
    RenderedText pause_text;

    // void* here is an underlying scene to render below the overlay
    PauseScene(SceneManager* manager, BaseScene* paused_scene)
        : manager(manager)
        , paused_scene(paused_scene)
    {
        overlay = manager->get_render_context()->create_overlay();
        pause_text = manager->get_render_context()->create_rendered_header_text("Paused");
    }

    virtual ~PauseScene()
    {
        SDL_DestroyTexture(overlay);
        pause_text.free();
    }

    virtual void update(InputState* input_state)
    {
        if (input_state->keys.get_state(SDLK_ESCAPE).was_pressed)
        {
            manager->set_scene(paused_scene);
            delete this;
            return;
        }
    }

    virtual void render(RenderContext* rc)
    {
        paused_scene->render(rc);
        rc->render_full_screen(overlay);
        rc->render_text(pause_text, vec2 {20, 20});
    }
};
