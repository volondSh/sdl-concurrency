#pragma once

#include <filesystem>

struct TTF_Font;
struct SDL_Color;
struct SDL_Renderer;

namespace sdl::core
{
  class TextRenderer final
  {
  public:
    explicit TextRenderer(SDL_Renderer* renderer, const std::filesystem::path& fontPath, float fontSize);
    ~TextRenderer();

    TextRenderer(const TextRenderer&)            = delete;
    TextRenderer& operator=(const TextRenderer&) = delete;
    TextRenderer(TextRenderer&&)                 = delete;
    TextRenderer& operator=(TextRenderer&&)      = delete;

    void renderText(const std::string& text, int x, int y, const SDL_Color& color) const;

    [[nodiscard]] bool valid() const;

  private:
    SDL_Renderer* m_pRenderer = nullptr;
    TTF_Font* m_pFont         = nullptr;
  };
}
