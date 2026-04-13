#include "Overlay.hpp"

#include <ECS/Components.h>

#include <imgui.h>

#include <algorithm>
#include <format>

using namespace sdl::widgets;

constexpr auto c_lineHeight  = 20.f;
constexpr auto c_margin      = 10.f;
constexpr auto c_windowWidth = 200.f;
constexpr auto c_graphWidth  = 180.f;
constexpr auto c_graphHeight = 45.f;

namespace
{
  template <typename... Ts>
  int countEntities(const entt::registry& reg)
  {
    if constexpr (sizeof...(Ts) == 1)
      return static_cast<int>(reg.view<Ts...>().size());
    else
    {
      auto count = 0;
      for (auto&& _ : reg.view<Ts...>())
        ++count;
      return count;
    }
  }
}

Overlay::Overlay() = default;

void Overlay::update(
    float deltaTime,
    const entt::registry& registry,
    const std::vector<sdl::core::ProfileEntry>& profiles)
{
  m_elapsed += deltaTime;
  ++m_frameCount;

  if (m_elapsed < 1.f)
    return;

  m_currentFps = static_cast<int>(m_frameCount / m_elapsed);

  m_lines.clear();
  m_lines.emplace_back(std::format("FPS: {}", m_currentFps));
  m_lines.emplace_back(std::format("Entities: {}", countEntities<ecs::Position>(registry)));
  m_lines.emplace_back(std::format("Moving: {}", countEntities<ecs::Velocity>(registry)));
  m_lines.emplace_back(std::format("Twinkle: {}", countEntities<ecs::Twinkle>(registry)));
  m_lines.emplace_back("Visible:");
  m_lines.emplace_back(std::format(" Entities: {}", countEntities<ecs::Visible>(registry)));
  m_lines.emplace_back(std::format(" Moving: {}", countEntities<ecs::Visible, ecs::Velocity>(registry)));
  m_lines.emplace_back(std::format(" Twinkle: {}", countEntities<ecs::Visible, ecs::Twinkle>(registry)));

  if (!profiles.empty())
  {
    m_lines.emplace_back("Systems:");
    auto totalMs = 0.f;

    for (const auto& p : profiles)
    {
      m_lines.emplace_back(std::format(" {}: {:.2f}ms", p.name, p.timeMs));
      totalMs += p.timeMs;
    }
    m_lines.emplace_back(std::format(" Total: {:.2f}ms", totalMs));
  }

  std::ranges::rotate(m_fpsHistory, m_fpsHistory.begin() + 1);
  m_fpsHistory.back() = static_cast<float>(m_currentFps);

  m_elapsed    = 0.f;
  m_frameCount = 0;
}

void Overlay::render()
{
  if (m_mode == OverlayMode::Hidden || m_lines.empty())
    return;

  const auto showGraph   = m_mode == OverlayMode::Graph;
  const auto graphHeight = showGraph ? c_graphHeight : 0.f;
  const auto textHeight  = c_lineHeight * static_cast<float>(m_lines.size());

  ImGui::SetNextWindowPos({c_margin, c_margin}, ImGuiCond_Always);
  ImGui::SetNextWindowSize({c_windowWidth, graphHeight + textHeight}, ImGuiCond_Always);
  ImGui::Begin(
      "Overlay",
      nullptr,
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse
          | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);

  auto y = c_margin;

  if (showGraph)
  {
    ImGui::SetCursorPosY(y);
    ImGui::PlotLines(
        "##fpsGraph",
        m_fpsHistory.data(),
        c_graphHistorySize,
        0,
        nullptr,
        0.f,
        70.f,
        {c_graphWidth, c_graphHeight});
    y += c_graphHeight + 5.f;
  }

  auto drawText = [&](const std::string& text)
  {
    ImGui::TextUnformatted(text.c_str());
    y += c_lineHeight;
  };

  for (const auto& line : m_lines)
    drawText(line);

  ImGui::End();
}

void Overlay::toggle()
{
  switch (m_mode)
  {
  case OverlayMode::Hidden:
    m_mode = OverlayMode::Text;
    break;
  case OverlayMode::Text:
    m_mode = OverlayMode::Graph;
    break;
  case OverlayMode::Graph:
    m_mode = OverlayMode::Hidden;
    break;
  }
}

bool Overlay::visible() const
{
  return m_mode != OverlayMode::Hidden;
}
