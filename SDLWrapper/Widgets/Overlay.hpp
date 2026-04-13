#pragma once

#include <SDLWrapper/Core/Profiler.hpp>

#include <entt/entt.hpp>

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace sdl::widgets
{
  enum class OverlayMode : std::uint8_t
  {
    Hidden,
    Text,
    Graph,
  };

  class Overlay final
  {
  public:
    explicit Overlay();

    void update(float deltaTime, const entt::registry& registry, const std::vector<sdl::core::ProfileEntry>& profiles);
    void render();
    void toggle();

    [[nodiscard]] bool visible() const;

  private:
    static constexpr int c_graphHistorySize = 120;

    OverlayMode m_mode = OverlayMode::Hidden;
    float m_elapsed    = 0.f;
    int m_frameCount   = 0;
    int m_currentFps   = 0;
    std::vector<std::string> m_lines;
    std::array<float, c_graphHistorySize> m_fpsHistory{};
  };
}
