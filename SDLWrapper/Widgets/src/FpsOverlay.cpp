#include "FpsOverlay.hpp"

#include <SDLWrapper/Core/TextRenderer.hpp>

#include <SDL3/SDL.h>

using namespace sdl::widgets;

FpsOverlay::FpsOverlay(const sdl::core::TextRenderer& textRenderer) : m_textRenderer{textRenderer}
{
}

void FpsOverlay::update(float deltaTime)
{
  m_elapsed += deltaTime;
  ++m_frameCount;

  if (m_elapsed < 1.f)
    return;

  const int fps  = static_cast<int>(m_frameCount / m_elapsed);
  m_text         = "FPS: " + std::to_string(fps);
  m_elapsed      = 0.f;
  m_frameCount   = 0;
}

void FpsOverlay::render()
{
  if (!m_visible || m_text.empty())
    return;

  constexpr auto green = SDL_Color{.r = 0, .g = 255, .b = 0, .a = 255};
  m_textRenderer.renderText(m_text, 10, 10, green);
}

void FpsOverlay::toggle()
{
  m_visible = !m_visible;
}

bool FpsOverlay::visible() const
{
  return m_visible;
}