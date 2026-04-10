#include "EventLoop.hpp"

#include <SDLWrapper/Widgets/Window.hpp>

#include <ECS/Components.h>
#include <ECS/Systems.hpp>

#include <SDL3/SDL.h>

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

EventLoop::EventLoop(sdl::widgets::Window& window, SceneConfig sceneConfig)
  : m_mainWindow{window},
    m_renderer{window},
    m_sceneConfig{sceneConfig},
    m_textRenderer{m_renderer.nativeHandle(), "resources/fonts/JetBrainsMono-Regular.ttf", 18},
    m_overlay{m_textRenderer}
{
  m_registry.ctx().emplace<ecs::Camera>(0.f, 0.f, 1.f);

  if (!m_textRenderer.valid())
    SPDLOG_WARN("TextRenderer failed to load - FPS overlay disabled");

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

void EventLoop::handleEvents(bool& running, float deltaTime)
{
  auto event   = SDL_Event{};
  auto& camera = m_registry.ctx().get<ecs::Camera>();

  while (SDL_PollEvent(&event))
  {
    if (event.type == SDL_EVENT_QUIT)
      running = false;
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
      running = false;
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_GRAVE)
      m_overlay.toggle();
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_F11)
      m_mainWindow.toggleFullscreen();
    if (event.type == SDL_EVENT_WINDOW_RESIZED)
    {
      if (!m_mainWindow.fullscreen())
        m_mainWindow.updateRestoreSize(event.window.data1, event.window.data2);

      const auto savedCamera = m_registry.ctx().get<ecs::Camera>();
      m_registry.clear();
      m_registry.ctx().emplace<ecs::Camera>(savedCamera.x, savedCamera.y, savedCamera.zoom);
      createScene();
    }
  }

  const auto* pKeys = SDL_GetKeyboardState(nullptr);
  if (pKeys[SDL_SCANCODE_W])
    camera.y -= m_sceneConfig.cameraPanSpeed * deltaTime;
  if (pKeys[SDL_SCANCODE_S])
    camera.y += m_sceneConfig.cameraPanSpeed * deltaTime;
  if (pKeys[SDL_SCANCODE_A])
    camera.x -= m_sceneConfig.cameraPanSpeed * deltaTime;
  if (pKeys[SDL_SCANCODE_D])
    camera.x += m_sceneConfig.cameraPanSpeed * deltaTime;
  if (pKeys[SDL_SCANCODE_Q])
    camera.zoom = std::clamp(
        camera.zoom * (1.f / m_sceneConfig.cameraZoomSpeed),
        m_sceneConfig.cameraZoomMin,
        m_sceneConfig.cameraZoomMax);
  if (pKeys[SDL_SCANCODE_E])
    camera.zoom = std::clamp(
        camera.zoom * m_sceneConfig.cameraZoomSpeed,
        m_sceneConfig.cameraZoomMin,
        m_sceneConfig.cameraZoomMax);
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

    movementSystem(m_registry, static_cast<float>(deltaTime), m_sceneConfig.worldWidth, m_sceneConfig.worldHeight);
    twinkleSystem(m_registry, static_cast<float>(deltaTime));
    transformSystem(m_registry, m_registry.ctx().get<Camera>(), m_mainWindow.width(), m_mainWindow.height());
    cullingSystem(m_registry, m_mainWindow.width(), m_mainWindow.height());
    renderScene();

    m_overlay.update(static_cast<float>(deltaTime), m_registry);
    m_overlay.render();
    m_renderer.present();

    const auto frameTime = (SDL_GetTicks() - now) / c_msPerSecond;
    if (frameTime < c_targetDelta)
      SDL_Delay(static_cast<std::uint32_t>((c_targetDelta - frameTime) * c_msPerSecond));
  }
}
