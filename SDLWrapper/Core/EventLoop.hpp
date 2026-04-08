#pragma once

#include "Renderer.hpp"

#include <entt/entt.hpp>

namespace sdl::widgets
{
  class Window;
}

namespace sdl::core
{
  struct SceneConfig
  {
    int totalStars;
    int movingStars;
    float worldWidth;
    float worldHeight;
    float cameraPanSpeed;
    float cameraZoomSpeed;
    float cameraZoomMin;
    float cameraZoomMax;
  };

  class EventLoop
  {
  public:
    explicit EventLoop(sdl::widgets::Window&, SceneConfig);
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
    SceneConfig m_sceneConfig;

    void createScene();
    void renderScene();

    void handleEvents(bool& running, float deltaTime);
  };
}
