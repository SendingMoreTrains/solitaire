class PauseScene : BaseScene
{
public:
    SceneManager* manager;
    BaseScene* paused_scene;

    // void* here is an underlying scene to render below the overlay
    PauseScene(SceneManager* manager, BaseScene* paused_scene)
        : manager(manager)
        , paused_scene(paused_scene)
    {}

    virtual void update(InputState*)
    {}

    virtual void render(RenderContext* rc)
    {
        paused_scene->render(rc);
    }
}
