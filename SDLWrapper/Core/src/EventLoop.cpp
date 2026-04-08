#include "EventLoop.hpp"

#include <SDLWrapper/Widgets/Window.hpp>

#include <ECS/Components.h>
#include <ECS/Systems.hpp>

#include <SDL3/SDL.h>

#include <cstdint>
#include <numbers>
#include <random>

using namespace sdl::core;
using namespace sdl::widgets;
using namespace ecs;

constexpr auto c_starsCount        = 2500;
constexpr auto c_movingStarsCount  = 500;
constexpr auto c_targetFPS         = 60;
constexpr auto c_targetDelta       = 1.0 / static_cast<double>(c_targetFPS);
constexpr auto c_msPerSecond       = 1000.0;
constexpr auto c_minStarBrightness = 100;
constexpr auto c_maxStarBrightness = 255;
constexpr auto c_blackColor        = SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255};
constexpr auto c_minStarSpeed      = 5.f;
constexpr auto c_maxStarSpeed      = 30.f;
constexpr auto c_cameraPanSpeed    = 300.f;
constexpr auto c_cameraZoomMin     = 0.1f;
constexpr auto c_cameraZoomMax     = 5.f;
constexpr auto c_worldSize         = 4000.f;

EventLoop::EventLoop(sdl::widgets::Window& window) : m_mainWindow{window}, m_renderer{window}
{
  m_registry.ctx().emplace<ecs::Camera>(0.f, 0.f, 1.f);
  createScene();
}

EventLoop::~EventLoop() = default;

void EventLoop::createScene()
{
  auto randomEngine   = std::mt19937(std::random_device{}());
  auto starPositionX  = std::uniform_real_distribution<float>(-c_worldSize * 0.5f, c_worldSize * 0.5f);
  auto starPositionY  = std::uniform_real_distribution<float>(-c_worldSize * 0.5f, c_worldSize * 0.5f);
  auto starBrightness = std::uniform_int_distribution<int>(c_minStarBrightness, c_maxStarBrightness);
  auto starSpeed      = std::uniform_real_distribution<float>(c_minStarSpeed, c_maxStarSpeed);
  auto starAngle      = std::uniform_real_distribution<float>(0.f, 2.f * std::numbers::pi_v<float>);

  for (int i = 0; i < c_starsCount; ++i)
  {
    const auto star = m_registry.create();
    m_registry.emplace<ecs::Position>(star, starPositionX(randomEngine), starPositionY(randomEngine));
    m_registry.emplace<ecs::ScreenPosition>(star);
    const auto brightness = static_cast<uint8_t>(starBrightness(randomEngine));
    m_registry.emplace<ecs::Color>(star, SDL_Color{.r = brightness, .g = brightness, .b = brightness, .a = 255});

    if (i < c_movingStarsCount)
    {
      const auto speed = starSpeed(randomEngine);
      const auto angle = starAngle(randomEngine);
      m_registry.emplace<ecs::Velocity>(star, std::cos(angle) * speed, std::sin(angle) * speed);
    }
  }
}

void EventLoop::renderScene()
{
  m_renderer.clearWithColor(c_blackColor);

  const auto view = m_registry.view<ecs::Position, ecs::ScreenPosition, ecs::Color>();
  const auto w    = m_mainWindow.width();
  const auto h    = m_mainWindow.height();

  for (const auto& [entity, pos, screen, color] : view.each())
  {
    if (screen.x < 0 || screen.x >= w || screen.y < 0 || screen.y >= h)
      continue;
    m_renderer.drawPoint(screen.x, screen.y, color.color);
  }
}

void EventLoop::handleEvents(bool& running)
{
  auto event   = SDL_Event{};
  auto& camera = m_registry.ctx().get<ecs::Camera>();

  while (SDL_PollEvent(&event))
  {
    if (event.type == SDL_EVENT_QUIT)
      running = false;
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
      running = false;
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_F11)
      m_mainWindow.toggleFullscreen();
    if (event.type == SDL_EVENT_WINDOW_RESIZED)
    {
      if (!m_mainWindow.fullscreen())
        m_mainWindow.updateRestoreSize(event.window.data1, event.window.data2);

      m_registry.clear();
      createScene();
    }
  }

  const auto* pKeys = SDL_GetKeyboardState(nullptr);
  if (pKeys[SDL_SCANCODE_W])
    camera.y -= c_cameraPanSpeed * static_cast<float>(c_targetDelta);
  if (pKeys[SDL_SCANCODE_S])
    camera.y += c_cameraPanSpeed * static_cast<float>(c_targetDelta);
  if (pKeys[SDL_SCANCODE_A])
    camera.x -= c_cameraPanSpeed * static_cast<float>(c_targetDelta);
  if (pKeys[SDL_SCANCODE_D])
    camera.x += c_cameraPanSpeed * static_cast<float>(c_targetDelta);
  if (pKeys[SDL_SCANCODE_Q])
    camera.zoom = std::clamp(camera.zoom * 0.95f, c_cameraZoomMin, c_cameraZoomMax);
  if (pKeys[SDL_SCANCODE_E])
    camera.zoom = std::clamp(camera.zoom * 1.05f, c_cameraZoomMin, c_cameraZoomMax);
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

    handleEvents(running);
    movementSystem(m_registry, static_cast<float>(deltaTime), c_worldSize, c_worldSize);
    transformSystem(m_registry, m_registry.ctx().get<Camera>(), m_mainWindow.width(), m_mainWindow.height());
    renderScene();
    m_renderer.present();

    const auto frameTime = (SDL_GetTicks() - now) / c_msPerSecond;
    if (frameTime < c_targetDelta)
      SDL_Delay(static_cast<std::uint32_t>((c_targetDelta - frameTime) * c_msPerSecond));
  }
}
