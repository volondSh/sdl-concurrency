#include "EventLoop.hpp"

#include <SDL3/SDL.h>

using namespace sdl::core;

void EventLoop::run(const TickCallback& on_tick)
{
  auto running = true;
  while (running)
  {
    SDL_Event event{};
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_EVENT_QUIT)
        running = false;
      if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
        running = false;
    }

    if (on_tick)
      on_tick();

    SDL_Delay(1);
  }
}
