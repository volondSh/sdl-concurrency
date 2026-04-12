#pragma once

#include <ECS/Components.h>

#include <entt/entt.hpp>

#include <cmath>

namespace ecs
{
  inline void movementSystem(entt::registry& reg, float dt, float worldWidth, float worldHeight)
  {
    constexpr auto epsilon = 1e-6f;
    const auto view        = reg.view<Position, Velocity>();
    for (const auto& [_, pos, vel] : view.each())
    {
      pos.x += vel.dx * dt;
      pos.y += vel.dy * dt;

      const auto halfW = worldWidth * 0.5f;
      const auto halfH = worldHeight * 0.5f;

      if (pos.x < -halfW - epsilon)
        pos.x += worldWidth;
      else if (pos.x + epsilon > halfW)
        pos.x -= worldWidth;

      if (pos.y < -halfH - epsilon)
        pos.y += worldHeight;
      else if (pos.y + epsilon > halfH)
        pos.y -= worldHeight;
    }
  }

  inline void transformSystem(entt::registry& reg, const Camera& camera, int windowWidth, int windowHeight)
  {
    const auto halfW = static_cast<float>(windowWidth) * 0.5f;
    const auto halfH = static_cast<float>(windowHeight) * 0.5f;

    auto view = reg.view<Position, ScreenPosition>();
    for (const auto& [_, pos, screenPos] : view.each())
    {
      screenPos.x = static_cast<int>(((pos.x - camera.x) * camera.zoom) + halfW);
      screenPos.y = static_cast<int>(((pos.y - camera.y) * camera.zoom) + halfH);
    }
  }

  inline void cullingSystem(entt::registry& reg, int windowWidth, int windowHeight)
  {
    auto view = reg.view<Position, ScreenPosition>();
    for (const auto [entity, _, screenPos] : view.each())
    {
      const auto inView =
          screenPos.x >= 0 && screenPos.x < windowWidth && screenPos.y >= 0 && screenPos.y < windowHeight;
      if (inView)
        reg.emplace_or_replace<Visible>(entity);
      else
        reg.remove<Visible>(entity);
    }
  }

  inline void twinkleSystem(entt::registry& reg, float dt)
  {
    auto view = reg.view<Color, Twinkle>();
    for (const auto [entity, color, twinkle] : view.each())
    {
      twinkle.phase += twinkle.frequency * dt;

      const auto factor = 0.5f + (0.5f * std::sin(twinkle.phase));
      const auto bright =
          static_cast<uint8_t>(color.color.r * (1.0f - twinkle.amplitude + (twinkle.amplitude * factor)));

      color.color.r = bright;
      color.color.g = bright;
      color.color.b = bright;
    }
  }
}
