#include "EventLoop.hpp"

#include <SDLWrapper/Widgets/Window.hpp>

#include <ECS/Components.h>

#include <SDL3/SDL.h>

#include <cstdint>
#include <random>

using namespace sdl::core;
using namespace sdl::widgets;

constexpr auto c_starsCount        = 2500;
constexpr auto c_targetFPS         = 60;
constexpr auto c_targetDelta       = 1.0 / static_cast<double>(c_targetFPS);
constexpr auto c_msPerSecond       = 1000.0;
constexpr auto c_minStarBrightness = 100;
constexpr auto c_maxStarBrightness = 255;
constexpr auto c_blackColor        = SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255};

EventLoop::EventLoop(sdl::widgets::Window& window) : m_mainWindow{window}, m_renderer{window}
{
  createStars(c_starsCount);
}

EventLoop::~EventLoop() = default;

void EventLoop::createStars(int count)
{
  auto randomEngine   = std::mt19937(std::random_device{}());
  auto starPositionX  = std::uniform_real_distribution<float>(0.f, static_cast<float>(m_mainWindow.width()));
  auto starPositionY  = std::uniform_real_distribution<float>(0.f, static_cast<float>(m_mainWindow.height()));
  auto starBrightness = std::uniform_int_distribution<int>(c_minStarBrightness, c_maxStarBrightness);

  for (int i = 0; i < count; ++i)
  {
    const auto star = m_registry.create();
    m_registry.emplace<ecs::Position>(star, starPositionX(randomEngine), starPositionY(randomEngine));
    const auto brightness = static_cast<uint8_t>(starBrightness(randomEngine));
    m_registry.emplace<ecs::Color>(star, SDL_Color{.r = brightness, .g = brightness, .b = brightness, .a = 255});
  }
}

void EventLoop::renderStars()
{
  m_renderer.clearWithColor(c_blackColor);

  auto view = m_registry.view<ecs::Position, ecs::Color>();
  for (const auto& [entity, pos, color] : view.each())
    m_renderer.drawPoint(static_cast<int>(pos.x), static_cast<int>(pos.y), color.color);
}

void EventLoop::handleEvents(bool& running)
{
  auto event = SDL_Event{};
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
      m_registry.clear();
      createStars(c_starsCount);
    }
  }
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
    renderStars();
    m_renderer.present();

    const auto frameTime = (SDL_GetTicks() - now) / c_msPerSecond;
    if (frameTime < c_targetDelta)
      SDL_Delay(static_cast<std::uint32_t>((c_targetDelta - frameTime) * c_msPerSecond));
  }
}
