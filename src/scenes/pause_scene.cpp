class PauseScene : public BaseScene
{
public:
    SceneManager* manager;
    BaseScene* paused_scene;

    SDL_Texture* overlay;
    RenderedText pause_text;

    Menu menu;

    enum Options
    {
        RESUME = 0,
        NEW = 1,
        QUIT = 2
    };

    PauseScene(SceneManager* manager, BaseScene* paused_scene)
        : manager(manager)
        , paused_scene(paused_scene)
        , menu(manager->get_render_context())
    {
        auto rc = manager->get_render_context();

        overlay = rc->create_overlay();
        pause_text = rc->create_rendered_header_text("Paused");

        menu.add_menu_option(rc, Options::RESUME, "Resume", vec2 {36, 70});
        menu.add_menu_option(rc, Options::NEW, "New Deal", vec2 {36, 90});
        menu.add_menu_option(rc, Options::QUIT, "Quit", vec2 {36, 110});
    }

    virtual ~PauseScene()
    {
        SDL_DestroyTexture(overlay);
        pause_text.free();
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

        switch (menu.update(input_state))
        {
        case (Options::RESUME):
        {
            return unpause();
        }
        case (Options::NEW):
        {
            paused_scene->reset();
            return unpause();
        }
        case (Options::QUIT):
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

        menu.render(rc);
    }
};
