#pragma once

#include "Renderer.hpp"

#include <entt/entt.hpp>

namespace app
{
  class ConfigManager;
}

namespace sdl::widgets
{
  class Window;
}

namespace sdl::core
{
  class EventLoop
  {
  public:
    explicit EventLoop(sdl::widgets::Window&);
    ~EventLoop();

    EventLoop(const EventLoop&)            = delete;
    EventLoop& operator=(const EventLoop&) = delete;
    EventLoop(EventLoop&&)                 = delete;
    EventLoop& operator=(EventLoop&&)      = delete;

    void run();

  private:
    sdl::widgets::Window& m_mainWindow;
    Renderer m_renderer;
    entt::registry m_registry;

    void createScene();
    void renderScene();

    void handleEvents(bool& running);
  };
}
