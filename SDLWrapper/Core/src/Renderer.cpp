#include "Renderer.hpp"

#include <SDLWrapper/Widgets/Window.hpp>

#include <SDL3/SDL.h>

#include <spdlog/spdlog.h>

using namespace sdl::core;

Renderer::Renderer(const sdl::widgets::Window& window) : m_pRenderer{SDL_CreateRenderer(window.nativeHandle(), nullptr)}
{
  if (!m_pRenderer)
  {
    SPDLOG_CRITICAL("Failed to create renderer: {}", SDL_GetError());
    throw std::runtime_error("No renderer");
  }
}

Renderer::~Renderer()
{
  if (m_pRenderer)
  {
    SDL_DestroyRenderer(m_pRenderer);
    m_pRenderer = nullptr;
  }
}

void Renderer::clear()
{
  SDL_RenderClear(m_pRenderer);
}

void Renderer::clearWithColor(const SDL_Color& color)
{
  SDL_SetRenderDrawColor(m_pRenderer, color.r, color.g, color.b, color.a);
  clear();
}

void Renderer::drawPoint(int x, int y)
{
  SDL_RenderPoint(m_pRenderer, x, y);
}

void Renderer::drawPoint(int x, int y, const SDL_Color& color)
{
  SDL_SetRenderDrawColor(m_pRenderer, color.r, color.g, color.b, color.a);
  drawPoint(x, y);
}

void Renderer::present()
{
  SDL_RenderPresent(m_pRenderer);
}

SDL_Renderer* Renderer::nativeHandle() const
{
  return m_pRenderer;
}
