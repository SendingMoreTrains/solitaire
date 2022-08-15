class BaseScene
{
public:
    virtual ~BaseScene() {};

    virtual void update(InputState*) = 0;
    virtual void render(RenderContext*) = 0;
};
