#pragma once

#include <cstdint>
#include <string_view>

struct SDL_Window;

namespace sdl::widgets
{
  class Window final
  {
  public:
    explicit Window(std::string_view title, int width, int height, std::uint64_t flags);
    ~Window();

    Window(const Window&)            = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&)                 = delete;
    Window& operator=(Window&&)      = delete;

    [[nodiscard]] bool valid() const noexcept;
    [[nodiscard]] SDL_Window* native_handle() const noexcept;

  private:
    SDL_Window* m_pWindow = nullptr;
  };

  std::uint64_t convertWindowSettingsToFlags(bool resizable, bool fullscreen);
}