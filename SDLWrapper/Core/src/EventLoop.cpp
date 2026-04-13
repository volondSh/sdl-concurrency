#include "EventLoop.hpp"

#include <App/ConfigManager.hpp>

#include <SDLWrapper/Widgets/Window.hpp>

#include <ECS/Components.h>
#include <ECS/Systems.hpp>

#include <SDL3/SDL.h>

#include <imgui.h>
#include <imgui_impl_sdl3.h>

#include <spdlog/spdlog.h>

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <numbers>
#include <random>

using namespace sdl::core;
using namespace sdl::widgets;
using namespace ecs;

constexpr auto c_targetFPS   = 60;
constexpr auto c_targetDelta = 1.0 / static_cast<double>(c_targetFPS);
constexpr auto c_msPerSecond = 1000.0;
constexpr auto c_blackColor  = SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255};

EventLoop::EventLoop(sdl::widgets::Window& window, SceneConfig sceneConfig, app::ConfigManager& configManager)
  : m_mainWindow{window},
    m_renderer{window},
    m_imgui{window.nativeHandle(), m_renderer.nativeHandle()},
    m_sceneConfig{sceneConfig},
    m_configManager{configManager},
    m_overlay{},
    m_menu{configManager}
{
  m_registry.ctx().emplace<ecs::Camera>(0.f, 0.f, 1.f);

  m_menu.setOnSave(
      [this]()
      {
        const auto& pending     = m_menu.pending();
        const auto starsChanged = pending.totalStars != m_sceneConfig.totalStars
                                  || pending.movingStars != m_sceneConfig.movingStars
                                  || pending.twinklingStars != m_sceneConfig.twinklingStars;
        const auto worldChanged =
            pending.worldWidth != m_sceneConfig.worldWidth || pending.worldHeight != m_sceneConfig.worldHeight;
        const auto cameraChanged = pending.cameraPanSpeed != m_sceneConfig.cameraPanSpeed
                                   || pending.cameraZoomSpeed != m_sceneConfig.cameraZoomSpeed
                                   || pending.cameraZoomMin != m_sceneConfig.cameraZoomMin
                                   || pending.cameraZoomMax != m_sceneConfig.cameraZoomMax;

        if (!starsChanged && !worldChanged && !cameraChanged)
        {
          SPDLOG_INFO("Settings unchanged, skipping save");
          return;
        }

        if (!m_configManager.save())
          SPDLOG_ERROR("Failed to save config");

        m_sceneConfig.totalStars      = m_configManager.totalStars();
        m_sceneConfig.movingStars     = m_configManager.movingStars();
        m_sceneConfig.twinklingStars  = m_configManager.twinklingStars();
        m_sceneConfig.worldWidth      = m_configManager.worldWidth();
        m_sceneConfig.worldHeight     = m_configManager.worldHeight();
        m_sceneConfig.cameraPanSpeed  = m_configManager.cameraPanSpeed();
        m_sceneConfig.cameraZoomSpeed = m_configManager.cameraZoomSpeed();
        m_sceneConfig.cameraZoomMin   = m_configManager.cameraZoomMin();
        m_sceneConfig.cameraZoomMax   = m_configManager.cameraZoomMax();

        const auto savedCamera = m_registry.ctx().get<ecs::Camera>();
        m_registry.clear();
        m_registry.ctx().emplace<ecs::Camera>(savedCamera.x, savedCamera.y, savedCamera.zoom);
        createScene();
      });

  createScene();
}

EventLoop::~EventLoop() = default;

void EventLoop::createScene()
{
  const auto worldWidth  = m_sceneConfig.worldWidth;
  const auto worldHeight = m_sceneConfig.worldHeight;
  auto randomEngine      = std::mt19937(std::random_device{}());
  auto starPositionX     = std::uniform_real_distribution<float>(-worldWidth * 0.5f, worldWidth * 0.5f);
  auto starPositionY     = std::uniform_real_distribution<float>(-worldHeight * 0.5f, worldHeight * 0.5f);
  auto starBrightness =
      std::uniform_int_distribution<int>(m_sceneConfig.starMinBrightness, m_sceneConfig.starMaxBrightness);
  auto starSpeed    = std::uniform_real_distribution<float>(m_sceneConfig.starMinSpeed, m_sceneConfig.starMaxSpeed);
  auto starAngle    = std::uniform_real_distribution<float>(0.f, 2.f * std::numbers::pi_v<float>);
  auto twinkleFreq  = std::uniform_real_distribution<float>(m_sceneConfig.twinkleMinFreq, m_sceneConfig.twinkleMaxFreq);
  auto twinkleAmp   = std::uniform_real_distribution<float>(m_sceneConfig.twinkleMinAmp, m_sceneConfig.twinkleMaxAmp);
  auto twinklePhase = std::uniform_real_distribution<float>(0.f, 6.2832f);

  for (unsigned int i = 0; i < static_cast<unsigned int>(m_sceneConfig.totalStars); ++i)
  {
    const auto star = m_registry.create();
    m_registry.emplace<ecs::Position>(star, starPositionX(randomEngine), starPositionY(randomEngine));
    m_registry.emplace<ecs::ScreenPosition>(star);
    m_registry.emplace<ecs::Visible>(star);
    const auto brightness = static_cast<uint8_t>(starBrightness(randomEngine));
    m_registry.emplace<ecs::Color>(star, SDL_Color{.r = brightness, .g = brightness, .b = brightness, .a = 255});

    if (i < static_cast<unsigned int>(m_sceneConfig.movingStars))
    {
      const auto speed = starSpeed(randomEngine);
      const auto angle = starAngle(randomEngine);
      m_registry.emplace<ecs::Velocity>(star, std::cos(angle) * speed, std::sin(angle) * speed);
    }

    if (i < static_cast<unsigned int>(m_sceneConfig.twinklingStars))
      m_registry
          .emplace<ecs::Twinkle>(star, twinklePhase(randomEngine), twinkleFreq(randomEngine), twinkleAmp(randomEngine));
  }
}

void EventLoop::renderScene()
{
  m_renderer.clearWithColor(c_blackColor);

  const auto view = m_registry.view<ecs::Visible, ecs::ScreenPosition, ecs::Color>();
  for (const auto& [_, screen, color] : view.each())
    m_renderer.drawPoint(screen.x, screen.y, color.color);
}

void EventLoop::processMenuEvents(bool& running, SDL_Event& event)
{
  if (event.type == SDL_EVENT_QUIT)
    running = false;

  if (event.type == SDL_EVENT_KEY_DOWN)
  {
    if (m_menu.capturingField() != sdl::widgets::ShortcutField::None)
    {
      if (event.key.scancode == SDL_SCANCODE_ESCAPE)
        m_menu.cancelCapture();
      else
        m_menu.applyCapture(event.key.scancode);
    }
    else if (event.key.key == SDLK_ESCAPE)
      m_menu.toggle();
  }
}

void EventLoop::processApplicationEvents(bool& running, SDL_Event& event)
{
  if (event.type == SDL_EVENT_QUIT)
    running = false;
  if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
    m_menu.toggle();
  if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == m_configManager.shortcutToggleOverlay())
    m_overlay.toggle();
  if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == m_configManager.shortcutToggleFullscreen())
    m_mainWindow.toggleFullscreen();
  if (event.type == SDL_EVENT_WINDOW_RESIZED)
    processWindowResize(event);
}

void EventLoop::processWindowResize(SDL_Event& event)
{
  if (!m_mainWindow.fullscreen())
    m_mainWindow.updateRestoreSize(event.window.data1, event.window.data2);

  const auto savedCamera = m_registry.ctx().get<ecs::Camera>();
  m_registry.clear();
  m_registry.ctx().emplace<ecs::Camera>(savedCamera.x, savedCamera.y, savedCamera.zoom);
  createScene();
}

void EventLoop::updateCameraInput(ecs::Camera& camera, float deltaTime)
{
  const auto* pKeys   = SDL_GetKeyboardState(nullptr);
  const auto panSpeed = m_sceneConfig.cameraPanSpeed;

  if (pKeys[m_configManager.shortcutMoveUp()])
    camera.y -= panSpeed * deltaTime;
  if (pKeys[m_configManager.shortcutMoveDown()])
    camera.y += panSpeed * deltaTime;
  if (pKeys[m_configManager.shortcutMoveLeft()])
    camera.x -= panSpeed * deltaTime;
  if (pKeys[m_configManager.shortcutMoveRight()])
    camera.x += panSpeed * deltaTime;
  if (pKeys[m_configManager.shortcutZoomIn()])
    camera.zoom = std::clamp(
        camera.zoom * m_sceneConfig.cameraZoomSpeed,
        m_sceneConfig.cameraZoomMin,
        m_sceneConfig.cameraZoomMax);
  if (pKeys[m_configManager.shortcutZoomOut()])
    camera.zoom = std::clamp(
        camera.zoom * (1.f / m_sceneConfig.cameraZoomSpeed),
        m_sceneConfig.cameraZoomMin,
        m_sceneConfig.cameraZoomMax);
}

void EventLoop::handleEvents(bool& running, float deltaTime)
{
  auto event = SDL_Event{};

  while (SDL_PollEvent(&event))
  {
    ImGui_ImplSDL3_ProcessEvent(&event);

    if (m_menu.isOpen())
      processMenuEvents(running, event);
    else
      processApplicationEvents(running, event);
  }

  if (!m_menu.isOpen())
    updateCameraInput(m_registry.ctx().get<ecs::Camera>(), deltaTime);
}

void EventLoop::run()
{
  auto running  = true;
  auto lastTick = SDL_GetTicks();

  while (running)
  {
    const auto now       = SDL_GetTicks();
    const auto deltaTime = (now - lastTick) / c_msPerSecond;
    lastTick             = now;

    handleEvents(running, static_cast<float>(deltaTime));

    {
      auto _ = m_profiler.profile("movement");
      movementSystem(m_registry, static_cast<float>(deltaTime), m_sceneConfig.worldWidth, m_sceneConfig.worldHeight);
    }
    {
      auto _ = m_profiler.profile("twinkle");
      twinkleSystem(m_registry, static_cast<float>(deltaTime));
    }
    {
      auto _ = m_profiler.profile("transform");
      transformSystem(m_registry, m_registry.ctx().get<Camera>(), m_mainWindow.width(), m_mainWindow.height());
    }
    {
      auto _ = m_profiler.profile("culling");
      cullingSystem(m_registry, m_mainWindow.width(), m_mainWindow.height());
    }
    {
      auto _ = m_profiler.profile("render");
      renderScene();
    }

    m_profiler.accumulate();
    m_overlay.update(static_cast<float>(deltaTime), m_registry, m_profiler.results());

    if (m_menu.isOpen())
    {
      m_imgui.newFrame();
      m_menu.render();
      m_imgui.render();
    }
    else if (m_overlay.visible())
    {
      m_imgui.newFrame();
      m_overlay.render();
      m_imgui.render();
    }

    m_renderer.present();

    m_profiler.reset();

    const auto frameTime = (SDL_GetTicks() - now) / c_msPerSecond;
    if (frameTime < c_targetDelta)
      SDL_Delay(static_cast<std::uint32_t>((c_targetDelta - frameTime) * c_msPerSecond));
  }
}
