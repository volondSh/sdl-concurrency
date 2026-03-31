#pragma once

#include <functional>

namespace sdl::core
{
  class EventLoop final
  {
  public:
    using TickCallback = std::function<void()>;

    void run(const TickCallback&);
  };
}
