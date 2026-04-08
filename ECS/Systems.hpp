#pragma once

#include <ECS/Components.h>

#include <entt/entt.hpp>

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
}
