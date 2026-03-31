#pragma once

#include <SDLWrapper/Core/Context.hpp>
#include <SDLWrapper/Core/EventLoop.hpp>
#include <SDLWrapper/Widgets/Window.hpp>

namespace app
{
  class Application final
  {
  public:
    Application();
    [[nodiscard]] bool is_ready() const noexcept;
    int exec();

  private:
    sdl::core::Context m_context;
    sdl::widgets::Window m_window;
    sdl::core::EventLoop m_eventLoop;
  };
}