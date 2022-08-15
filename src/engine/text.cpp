class OutlineFont
{
public:
    OutlineFont(const char* font_path, int font_size, int outline_width)
        : base_font{}
        , outline_font{}
        , outline_width{ outline_width }
    {
        base_font = TTF_OpenFont(font_path, font_size);

        outline_font = TTF_OpenFont(font_path, font_size);
        TTF_SetFontOutline(outline_font, outline_width);
    }

    SDL_Surface* render_outlined_text(const char* text)
    {
        SDL_Surface* bg_surface = TTF_RenderText_Solid(outline_font, text, black);
        SDL_Surface* fg_surface = TTF_RenderText_Solid(base_font, text, white);
        SDL_Rect render_rect = {outline_width, outline_width, fg_surface->w, fg_surface->h};


        Uint32 rmask, gmask, bmask, amask;
        /* SDL interprets each pixel as a 32-bit number, so our masks must depend
           on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
#else
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0xff000000;
#endif
        // Create a surface that will be the returned result
        SDL_Surface* test = SDL_CreateRGBSurface(0,bg_surface->w,bg_surface->h,32,rmask, gmask, bmask, amask);

        // Blit our fg and bg surfaces onto the result
        SDL_BlitSurface(bg_surface, NULL, test, NULL);
        SDL_BlitSurface(fg_surface, NULL, test, &render_rect);

        SDL_FreeSurface(fg_surface);
        SDL_FreeSurface(bg_surface);

        return test;
    }

private:
    TTF_Font* base_font;
    TTF_Font* outline_font;
    int outline_width;

    const SDL_Color white{ 0xFF, 0xFF, 0xFF };
    const SDL_Color black{ 0x00, 0x00, 0x00 };
};

struct OutlineTextTexture
{
    SDL_Texture* texture;
    SDL_Rect dimensions;
};

struct TextGenerator
{};
