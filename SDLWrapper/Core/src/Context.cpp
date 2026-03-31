#include "Context.hpp"

#include <SDL3/SDL.h>

#include <spdlog/spdlog.h>

using namespace sdl::core;

Context::Context(unsigned int flags)
{
  if (SDL_Init(flags))
  {
    m_initialized = true;
    SPDLOG_INFO("SDL3 initialized with flags={}", flags);
    return;
  }

  SPDLOG_ERROR("SDL3 init failed: {}", SDL_GetError());
}

Context::~Context()
{
  if (m_initialized)
  {
    SDL_Quit();
    SPDLOG_INFO("SDL3 shutdown complete");
  }
}

bool Context::initialized() const noexcept
{
  return m_initialized;
}

std::string_view Context::init_error() const noexcept
{
  if (m_initialized)
    return {};

  return SDL_GetError();
}
