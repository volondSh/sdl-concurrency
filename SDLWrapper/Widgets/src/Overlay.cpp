#include "Overlay.hpp"

#include <ECS/Components.h>

#include <SDL3/SDL.h>
#include <SDLWrapper/Core/TextRenderer.hpp>

#include <format>

using namespace sdl::widgets;

namespace
{
  template <typename... Ts>
  int countEntities(const entt::registry& reg)
  {
    auto count = 0;
    for (auto&& _ : reg.view<Ts...>())
      ++count;
    return count;
  }
}

Overlay::Overlay(const sdl::core::TextRenderer& textRenderer) : m_textRenderer{textRenderer}
{
}

void Overlay::update(
    float deltaTime,
    const entt::registry& registry,
    const std::vector<sdl::core::ProfileEntry>& profiles)
{
  m_elapsed += deltaTime;
  ++m_frameCount;

  if (m_elapsed < 1.f)
    return;

  m_lines.clear();
  m_lines.emplace_back(std::format("FPS: {}", static_cast<int>(m_frameCount / m_elapsed)));
  m_lines.emplace_back(std::format("Entities: {}", static_cast<int>(registry.view<ecs::Position>().size())));
  m_lines.emplace_back(std::format("Moving: {}", static_cast<int>(registry.view<ecs::Velocity>().size())));
  m_lines.emplace_back(std::format("Twinkle: {}", static_cast<int>(registry.view<ecs::Twinkle>().size())));
  m_lines.emplace_back("Visible:");
  m_lines.emplace_back(std::format(" Entities: {}", static_cast<int>(registry.view<ecs::Visible>().size())));
  m_lines.emplace_back(std::format(" Moving: {}", countEntities<ecs::Visible, ecs::Velocity>(registry)));
  m_lines.emplace_back(std::format(" Twinkle: {}", countEntities<ecs::Visible, ecs::Twinkle>(registry)));

  if (!profiles.empty())
  {
    m_lines.emplace_back("Profiles:");

    auto totalMs = 0.f;
    for (const auto& p : profiles)
    {
      m_lines.emplace_back(std::format(" {}: {:.2f}ms", p.name, p.timeMs));
      totalMs += p.timeMs;
    }
    m_lines.emplace_back(std::format(" Total: {:.2f}ms", totalMs));
  }

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
