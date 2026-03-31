#include "Application.hpp"

#include <SDL3/SDL.h>

#include <spdlog/spdlog.h>

using namespace app;

Application::Application()
  : m_context(SDL_INIT_EVENTS | SDL_INIT_VIDEO),
    m_window("sdl-concurrency", 1920, 1080, SDL_WINDOW_RESIZABLE)
{
}

bool Application::is_ready() const noexcept
{
  return m_context.initialized() && m_window.valid();
}

int Application::exec()
{
  if (!m_context.initialized())
  {
    SPDLOG_CRITICAL("Unable to start SDL3: {}", m_context.init_error());
    return 1;
  }
  if (!m_window.valid())
  {
    SPDLOG_CRITICAL("Unable to create window");
    return 1;
  }

  SPDLOG_INFO("SDL window created. Close window or press Escape to exit.");
  m_eventLoop.run(nullptr);
  return 0;
}
