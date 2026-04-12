#pragma once

struct SDL_Window;
struct SDL_Renderer;

namespace sdl::core
{
  class ImGuiRenderer final
  {
  public:
    explicit ImGuiRenderer(SDL_Window* window, SDL_Renderer* renderer);
    ~ImGuiRenderer();

    ImGuiRenderer(const ImGuiRenderer&)            = delete;
    ImGuiRenderer& operator=(const ImGuiRenderer&) = delete;
    ImGuiRenderer(ImGuiRenderer&&)                 = delete;
    ImGuiRenderer& operator=(ImGuiRenderer&&)      = delete;

    void newFrame();
    void render();

    [[nodiscard]] SDL_Window* window() const;
    [[nodiscard]] SDL_Renderer* renderer() const;

  private:
    SDL_Window* m_pWindow     = nullptr;
    SDL_Renderer* m_pRenderer = nullptr;
  };
}
