#include "Context.hpp"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <spdlog/spdlog.h>

using namespace sdl::core;

Context::Context(unsigned int flags)
{
  if (SDL_Init(flags))
  {
    if (TTF_Init())
    {
      m_ttfInitialized = true;
      m_initialized    = true;
      SPDLOG_INFO("SDL3 and SDL_ttf initialized with flags={}", flags);
      return;
    }

    SPDLOG_ERROR("SDL_ttf init failed: {}", SDL_GetError());
    SDL_Quit();
  }

  SPDLOG_ERROR("SDL3 init failed: {}", SDL_GetError());
}

Context::~Context()
{
  if (m_initialized)
  {
    if (m_ttfInitialized)
    {
      TTF_Quit();
      SPDLOG_INFO("SDL_ttf shutdown complete");
    }
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
