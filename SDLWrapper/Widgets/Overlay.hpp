#pragma once

#include <entt/entt.hpp>

#include <string>
#include <vector>

namespace sdl::core
{
  class TextRenderer;
}

namespace sdl::widgets
{
  class Overlay final
  {
  public:
    explicit Overlay(const sdl::core::TextRenderer& textRenderer);

    void update(float deltaTime, const entt::registry& registry);
    void render();
    void toggle();

    [[nodiscard]] bool visible() const;

  private:
    const sdl::core::TextRenderer& m_textRenderer;
    bool m_visible   = false;
    float m_elapsed  = 0.f;
    int m_frameCount = 0;
    std::vector<std::string> m_lines;
  };
}
