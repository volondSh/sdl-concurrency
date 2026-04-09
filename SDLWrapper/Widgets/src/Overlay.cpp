#include "Overlay.hpp"

#include <ECS/Components.h>

#include <SDL3/SDL.h>
#include <SDLWrapper/Core/TextRenderer.hpp>

#include <format>

using namespace sdl::widgets;

Overlay::Overlay(const sdl::core::TextRenderer& textRenderer) : m_textRenderer{textRenderer}
{
}

void Overlay::update(float deltaTime, const entt::registry& registry)
{
  m_elapsed += deltaTime;
  ++m_frameCount;

  if (m_elapsed < 1.f)
    return;

  m_lines.clear();
  m_lines.reserve(3);
  m_lines.push_back(std::format("FPS: {}", static_cast<int>(m_frameCount / m_elapsed)));
  m_lines.push_back(std::format("Entities: {}", static_cast<int>(registry.view<ecs::Position>().size())));
  m_lines.push_back(std::format("Visible: {}", static_cast<int>(registry.view<ecs::Visible>().size())));

  m_elapsed    = 0.f;
  m_frameCount = 0;
}

void Overlay::render()
{
  if (!m_visible || m_lines.empty())
    return;

  constexpr auto green     = SDL_Color{.r = 0, .g = 255, .b = 0, .a = 255};
  constexpr int lineHeight = 20;
  constexpr int margin     = 10;

  auto y = margin;
  for (const auto& line : m_lines)
  {
    m_textRenderer.renderText(line, margin, y, green);
    y += lineHeight;
  }
}

void Overlay::toggle()
{
  m_visible = !m_visible;
}

bool Overlay::visible() const
{
  return m_visible;
}
