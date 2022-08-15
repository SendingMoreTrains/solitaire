struct Sprite {
    SDL_Texture* texture;
    rect src_rect;
};

class SpriteSheet {
public:
    SpriteSheet(SDL_Texture* texture, int sprite_width, int sprite_height)
        : _texture{texture}
        , _spr_width{sprite_width}
        , _spr_height{sprite_height}
    {}

    ~SpriteSheet() { SDL_DestroyTexture(_texture); };

    Sprite createSprite(int sprite_column, int sprite_row)
    {
        SDL_Rect src_rect{sprite_column * _spr_width, sprite_row * _spr_height, _spr_width, _spr_height};
        return Sprite{_texture, src_rect};
    }

    int get_sprite_width() { return _spr_width; }
    int get_sprite_height() { return _spr_height; }

private:
    SDL_Texture* _texture;
    int _spr_width, _spr_height;
};


class RenderContext {
public:
    RenderContext(const char* title, int game_width, int game_height, int scale = 1)
        : _title{title}
        , _game_width{game_width}
        , _game_height{game_height}
        , _window_scale{scale}
        , _window{nullptr}
        , _renderer{nullptr}
    {}

    bool init()
    {
        _window = SDL_CreateWindow(_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _game_width * _window_scale,
                                   _game_height * _window_scale,
                                   SDL_WINDOW_SHOWN);
        if (_window == nullptr) {
            std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        _renderer = SDL_CreateRenderer(_window, -1, (SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED));
        if (_renderer == nullptr) {
            std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
        SDL_RenderSetLogicalSize(_renderer, _game_width, _game_height);

        return true;
    }

    ~RenderContext()
    {
        SDL_DestroyRenderer(_renderer);
        SDL_DestroyWindow(_window);
    }

    SDL_Texture* loadTexture(const char* file)
    {
        return IMG_LoadTexture(_renderer, file);
    }

    void setDrawColor(int r, int g, int b)
    {
        SDL_SetRenderDrawColor(_renderer, r, g, b, 0xFF);
    }

    void clearScreen()
    {
        SDL_RenderClear(_renderer);
    }

    void renderSprite(Sprite sprite, rect* dst_rect)
    {
        SDL_RenderCopy(_renderer, sprite.texture, &sprite.src_rect, dst_rect);
    }

    void renderSprite(Sprite sprite, vec2 pos)
    {
        rect dst_rect{ pos.x, pos.y, sprite.src_rect.w, sprite.src_rect.h };
        SDL_RenderCopy(_renderer, sprite.texture, &sprite.src_rect, &dst_rect);
    }

    void present()
    {
        SDL_RenderPresent(_renderer);
    }

    const char* _title;
    int _game_width, _game_height;
    int _window_scale;
    SDL_Window* _window;
    SDL_Renderer* _renderer;
};
