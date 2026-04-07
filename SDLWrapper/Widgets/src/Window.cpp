#include "Window.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

#include <spdlog/spdlog.h>
#include <utility>

using namespace sdl::widgets;

namespace
{
  std::pair<int, int> windowSize(SDL_Window* pWindow)
  {
    if (!pWindow)
      return {0, 0};

    auto width  = 0;
    auto height = 0;
    SDL_GetWindowSize(pWindow, &width, &height);

    return {width, height};
  }
}

Window::Window(std::string_view title, int width, int height, std::uint64_t flags)
  : m_pWindow{SDL_CreateWindow(title.data(), width, height, flags)}
{
  if (!m_pWindow)
    SPDLOG_ERROR("Failed to create SDL window: {}", SDL_GetError());
}

Window::~Window()
{
  if (m_pWindow)
  {
    SDL_DestroyWindow(m_pWindow);
    m_pWindow = nullptr;
  }
}

bool Window::valid() const noexcept
{
  return m_pWindow != nullptr;
}

SDL_Window* Window::nativeHandle() const noexcept
{
  return m_pWindow;
}

void Window::setWidth(int width)
{
  SDL_SetWindowSize(m_pWindow, width, windowSize(m_pWindow).second);
}

int Window::width() const
{
  return windowSize(m_pWindow).first;
}

void Window::setHeight(int height)
{
  SDL_SetWindowSize(m_pWindow, windowSize(m_pWindow).first, height);
}

int Window::height() const
{
  return windowSize(m_pWindow).second;
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
