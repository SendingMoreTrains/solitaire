class BaseScene
{
public:
    virtual void on_create(RenderContext*, void*) = 0;

    virtual void update(InputState*) = 0;
    virtual void render(RenderContext*) = 0;
}
