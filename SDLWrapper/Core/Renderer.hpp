#pragma once

#include <SDL3/SDL.h>

struct SDL_Renderer;

namespace sdl::widgets
{
  class Window;
}

namespace sdl::core
{
  class Renderer final
  {
  public:
    explicit Renderer(const sdl::widgets::Window& window);
    ~Renderer();

    Renderer(const Renderer&)            = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&)                 = delete;
    Renderer& operator=(Renderer&&)      = delete;

    void clear();
    void clearWithColor(const SDL_Color&);
    void drawPoint(int x, int y);
    void drawPoint(int x, int y, const SDL_Color&);
    void present();

    [[nodiscard]] SDL_Renderer* nativeHandle() const { return m_pRenderer; }

  private:
    SDL_Renderer* m_pRenderer = nullptr;
  };
}