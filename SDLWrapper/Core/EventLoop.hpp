#pragma once

#include "ImGuiRenderer.hpp"
#include "Profiler.hpp"
#include "Renderer.hpp"

#include <SDLWrapper/Widgets/Menu.hpp>
#include <SDLWrapper/Widgets/Overlay.hpp>

#include <entt/entt.hpp>

namespace app
{
  class ConfigManager;
}

namespace ecs
{
  struct Camera;
}

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
    int starMinBrightness;
    int starMaxBrightness;
    float starMinSpeed;
    float starMaxSpeed;
    float worldWidth;
    float worldHeight;
    float cameraPanSpeed;
    float cameraZoomSpeed;
    float cameraZoomMin;
    float cameraZoomMax;
    int twinklingStars;
    float twinkleMinFreq;
    float twinkleMaxFreq;
    float twinkleMinAmp;
    float twinkleMaxAmp;
  };

  class EventLoop
  {
  public:
    explicit EventLoop(sdl::widgets::Window&, SceneConfig, app::ConfigManager&);
    ~EventLoop();

    EventLoop(const EventLoop&)            = delete;
    EventLoop& operator=(const EventLoop&) = delete;
    EventLoop(EventLoop&&)                 = delete;
    EventLoop& operator=(EventLoop&&)      = delete;

    void run();

  private:
    void createScene();
    void syncRegistries();
    void renderScene();

    void handleEvents(bool& running, float deltaTime);
    void processMenuEvents(bool& running, SDL_Event& event);
    void processApplicationEvents(bool& running, SDL_Event& event);
    void processWindowResize(SDL_Event& event);
    void updateCameraInput(ecs::Camera& camera, float deltaTime);

    sdl::widgets::Window& m_mainWindow;
    Renderer m_renderer;
    ImGuiRenderer m_imgui;
    entt::registry m_logicRegistry;
    entt::registry m_renderRegistry;
    std::vector<entt::entity> m_logicEntities;
    std::vector<entt::entity> m_renderEntities;
    SceneConfig m_sceneConfig;
    app::ConfigManager& m_configManager;
    sdl::widgets::Overlay m_overlay;
    sdl::widgets::Menu m_menu;
    Profiler m_profiler;
  };
}
