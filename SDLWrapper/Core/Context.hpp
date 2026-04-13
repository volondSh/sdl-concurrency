#pragma once

#include <string_view>

namespace sdl::core
{
  class Context final
  {
  public:
    explicit Context(unsigned int flags);
    ~Context();

    Context(const Context&)            = delete;
    Context& operator=(const Context&) = delete;
    Context(Context&&)                 = delete;
    Context& operator=(Context&&)      = delete;

    [[nodiscard]] bool initialized() const noexcept;
    [[nodiscard]] std::string_view init_error() const noexcept;

  private:
    bool m_initialized    = false;
  };
}
