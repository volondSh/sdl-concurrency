#pragma once

#include <SDL3/SDL.h>

namespace ecs
{
  struct Position
  {
    float x = 0.0f;
    float y = 0.0f;
  };

  struct Velocity
  {
    float dx = 0.0f;
    float dy = 0.0f;
  };

  struct Color
  {
    SDL_Color color = {.r = 0, .g = 0, .b = 0, .a = 0};
  };
}