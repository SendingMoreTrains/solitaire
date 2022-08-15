class SceneManager
{
private:
    BaseScene* active_scene;
    RenderContext* render_context;

public:
    SceneManager(RenderContext* rc)
        : render_context(rc)
    {}

    void update(InputState* input_state)
    {
        active_scene->update(input_state);
    }

    void render()
    {
        active_scene->render(render_context);
    }

    void set_scene(BaseScene* new_scene) { active_scene = new_scene; }
    BaseScene* get_scene() { return active_scene; }

    RenderContext* get_render_context() { return render_context; }
}
