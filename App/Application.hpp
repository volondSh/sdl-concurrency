#pragma once

#include <SDLWrapper/Core/Context.hpp>
#include <SDLWrapper/Core/EventLoop.hpp>
#include <SDLWrapper/Widgets/Window.hpp>

namespace app
{
  class ConfigManager;

  class Application final
  {
  public:
    explicit Application(ConfigManager&);
    ~Application();

    Application(const Application&)            = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&)                 = delete;
    Application& operator=(Application&&)      = delete;

    [[nodiscard]] bool ready() const noexcept;
    int exec();

  private:
    sdl::core::Context m_context;
    sdl::widgets::Window m_mainWindow;
    sdl::core::EventLoop m_eventLoop;
    ConfigManager& m_configManager;
  };
}