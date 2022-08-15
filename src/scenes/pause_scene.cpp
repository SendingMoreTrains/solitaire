class PauseScene : public BaseScene
{
public:
    SceneManager* manager;
    BaseScene* paused_scene;

    SDL_Texture* overlay;
    RenderedText pause_text;

    RenderedText text_resume;
    RenderedText text_new_game;
    RenderedText text_quit;

    // void* here is an underlying scene to render below the overlay
    PauseScene(SceneManager* manager, BaseScene* paused_scene)
        : manager(manager)
        , paused_scene(paused_scene)
    {
        auto rc = manager->get_render_context();

        overlay = rc->create_overlay();
        pause_text = rc->create_rendered_header_text("Paused");

        text_resume = rc->create_rendered_text("Resume");
        text_resume.set_position(vec2 {36, 70});

        text_new_game = rc->create_rendered_text("New Deal");
        text_new_game.set_position(vec2 {36, 90});

        text_quit = rc->create_rendered_text("Quit");
        text_quit.set_position(vec2 {36, 110});
    }

    virtual ~PauseScene()
    {
        SDL_DestroyTexture(overlay);
        pause_text.free();
        text_resume.free();
        text_new_game.free();
        text_quit.free();
    }

    void unpause()
    {
        manager->set_scene(paused_scene);
        delete this;
    }

    virtual void update(InputState* input_state)
    {
        if (input_state->keys.get_state(SDLK_ESCAPE).was_pressed)
        {
            return unpause();
        }

        if (input_state->mouse.left.was_pressed)
        {
            if (text_resume.is_within_bounds(input_state->mouse.pos))
            {
                return unpause();
            }
            else if (text_new_game.is_within_bounds(input_state->mouse.pos))
            {
                return unpause();
            }
            else if (text_quit.is_within_bounds(input_state->mouse.pos))
            {
                send_quit_event();
                return;
            }
        }
    }

    virtual void render(RenderContext* rc)
    {
        paused_scene->render(rc);
        rc->render_full_screen(overlay);
        rc->render_text(pause_text, vec2 {30, 30});

        rc->render_text(text_resume);
        rc->render_text(text_new_game);
        rc->render_text(text_quit);
    }
};
