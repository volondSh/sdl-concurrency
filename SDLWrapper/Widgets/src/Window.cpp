#include "Window.hpp"

#include <SDL3/SDL.h>

#include <spdlog/spdlog.h>

using namespace sdl::widgets;

Window::Window(std::string_view title, int width, int height, unsigned long long flags)
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
