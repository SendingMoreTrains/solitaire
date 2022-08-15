struct ButtonState {
    bool is_down, was_pressed, was_released;
};

struct MouseState {
    vec2 pos;
    ButtonState left, right;
};

struct InputState {
    MouseState mouse;
};


void processFrameStart(InputState* inputState)
{
    inputState->mouse.left.was_released = false;
    inputState->mouse.left.was_pressed = false;
    inputState->mouse.right.was_released = false;
    inputState->mouse.right.was_pressed = false;
}

void processSdlEvent(InputState* inputState, SDL_Event* event)
{
    switch (event->type) {
    case SDL_MOUSEMOTION:
        inputState->mouse.pos.x = event->motion.x;
        inputState->mouse.pos.y = event->motion.y;
        break;

    case SDL_MOUSEBUTTONUP:
        inputState->mouse.pos.x = event->button.x;
        inputState->mouse.pos.y = event->button.y;

        if (event->button.button == SDL_BUTTON_LEFT) {
            inputState->mouse.left.was_released = true;
            inputState->mouse.left.is_down = false;
        }

        if (event->button.button == SDL_BUTTON_RIGHT) {
            inputState->mouse.right.was_released = true;
            inputState->mouse.right.is_down = false;
        }
        break;

    case SDL_MOUSEBUTTONDOWN:
        inputState->mouse.pos.x = event->button.x;
        inputState->mouse.pos.y = event->button.y;

        if (event->button.button == SDL_BUTTON_LEFT) {
            inputState->mouse.left.was_pressed = true;
            inputState->mouse.left.is_down = true;
        }

        if (event->button.button == SDL_BUTTON_RIGHT) {
            inputState->mouse.right.was_pressed = true;
            inputState->mouse.right.is_down = true;;
        }
        break;
    }
}
