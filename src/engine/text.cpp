struct OutlineFont
{
    TTF_Font* base_font;
    TTF_Font* outline_font;
    int outline_width;

    const SDL_Color text_color{ 0xFF, 0xFF, 0xFF };
    const SDL_Color outline_color{ 0x00, 0x00, 0x00 };

    OutlineFont(const char* font_path, int font_size, int outline_width)
        : base_font{}
        , outline_font{}
        , outline_width{ outline_width }
    {
        base_font = TTF_OpenFont(font_path, font_size);

        outline_font = TTF_OpenFont(font_path, font_size);
        TTF_SetFontOutline(outline_font, outline_width);
    }

    SDL_Surface* draw_outlined_text(const char* text)
    {
        SDL_Surface* bg_surface = TTF_RenderText_Solid(outline_font, text, outline_color);
        SDL_Surface* fg_surface = TTF_RenderText_Solid(base_font, text, text_color);
        SDL_Rect render_rect = {outline_width, outline_width, fg_surface->w, fg_surface->h};

        // Create a surface that will be the returned result
        SDL_Surface* test = CreateSDL_RGBA_Surface(bg_surface->w, bg_surface->h);

        // Blit our fg and bg surfaces onto the result
        SDL_BlitSurface(bg_surface, NULL, test, NULL);
        SDL_BlitSurface(fg_surface, NULL, test, &render_rect);

        SDL_FreeSurface(fg_surface);
        SDL_FreeSurface(bg_surface);

        return test;
    }
};


struct RenderedText
{
    SDL_Rect area;
    SDL_Texture* texture;
};
