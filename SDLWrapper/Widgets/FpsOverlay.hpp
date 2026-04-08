#pragma once

#include <string>

namespace sdl::core
{
  class TextRenderer;
}

namespace sdl::widgets
{
  class FpsOverlay final
  {
  public:
    explicit FpsOverlay(const sdl::core::TextRenderer& textRenderer);

    void update(float deltaTime);
    void render();
    void toggle();

    [[nodiscard]] bool visible() const;

  private:
    const sdl::core::TextRenderer& m_textRenderer;
    bool m_visible   = false;
    float m_elapsed  = 0.f;
    int m_frameCount = 0;
    std::string m_text;
  };
}
