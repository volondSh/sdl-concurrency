#include "Window.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

#include <spdlog/spdlog.h>
#include <utility>

using namespace sdl::widgets;

Window::Window(std::string_view title, int width, int height, std::uint64_t flags)
  : m_pWindow{SDL_CreateWindow(title.data(), width, height, flags)}
{
  if (!m_pWindow)
    SPDLOG_ERROR("Failed to create SDL window: {}", SDL_GetError());
}

Window::~Window()
{
  if (m_pWindow)
    SDL_DestroyWindow(m_pWindow);
}

bool Window::valid() const noexcept
{
  return m_pWindow != nullptr;
}

SDL_Window* Window::native_handle() const noexcept
{
  return m_pWindow;
}

std::uint64_t sdl::widgets::convertWindowSettingsToFlags(bool resizable, bool fullscreen)
{
  auto flags = std::uint64_t{0};
  if (resizable)
    flags |= SDL_WINDOW_RESIZABLE;
  if (fullscreen)
    flags |= SDL_WINDOW_FULLSCREEN;
  return flags;
}