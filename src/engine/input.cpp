struct ButtonState {
    bool isDown, wasPressed, wasReleased;
};

struct MouseState {
    int x, y;
    ButtonState left, right;
};


void processFrameStart(MouseState* mouseState)
{
    mouseState->left.wasReleased = false;
    mouseState->left.wasPressed = false;
    mouseState->right.wasReleased = false;
    mouseState->right.wasPressed = false;
}

void processInput(MouseState* mouseState, SDL_Event* event)
{
    switch (event->type) {
    case SDL_MOUSEMOTION:
        mouseState->x = event->motion.x;
        mouseState->y = event->motion.y;
        break;

    case SDL_MOUSEBUTTONUP:
        mouseState->x = event->button.x;
        mouseState->y = event->button.y;

        if (event->button.button == SDL_BUTTON_LEFT) {
            mouseState->left.wasReleased = true;
            mouseState->left.isDown = false;
        }

        if (event->button.button == SDL_BUTTON_RIGHT) {
            mouseState->right.wasReleased = true;
            mouseState->right.isDown = false;
        }
        break;

    case SDL_MOUSEBUTTONDOWN:
        mouseState->x = event->button.x;
        mouseState->y = event->button.y;

        if (event->button.button == SDL_BUTTON_LEFT) {
            mouseState->left.wasPressed = true;
            mouseState->left.isDown = true;
        }

        if (event->button.button == SDL_BUTTON_RIGHT) {
            mouseState->right.wasPressed = true;
            mouseState->right.isDown = true;;
        }
        break;
    }
}
