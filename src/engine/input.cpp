struct ButtonState {
    bool is_down{ false };
    bool was_pressed{ false };
    bool was_released{ false };
};

struct MouseState {
    vec2 pos;
    ButtonState left, right;
};

struct KeyboardState
{
    ButtonState& get_state(SDL_Keycode keycode)
    {
        return keys[keycode];
    }

    std::map<SDL_Keycode, ButtonState> keys{};
};

struct InputState {
    MouseState mouse;
    KeyboardState keys;
};


void processFrameStart(InputState* inputState)
{
    inputState->mouse.left.was_released = false;
    inputState->mouse.left.was_pressed = false;
    inputState->mouse.right.was_released = false;
    inputState->mouse.right.was_pressed = false;

    for (auto& it : inputState->keys.keys)
    {
        it.second.was_released = false;
        it.second.was_pressed = false;
    }
}

void processSdlEvent(InputState* inputState, SDL_Event* event)
{
    switch (event->type)
    {
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

    case SDL_KEYDOWN:
        {
            ButtonState& state{ inputState->keys.keys[event->key.keysym.sym] };

            state.was_pressed = !state.is_down;
            state.is_down = true;
            break;
        }

    case SDL_KEYUP:
        {
            ButtonState& state{ inputState->keys.keys[event->key.keysym.sym] };

            state.was_released = state.is_down;
            state.is_down = false;
            break;
        }
    }
}
