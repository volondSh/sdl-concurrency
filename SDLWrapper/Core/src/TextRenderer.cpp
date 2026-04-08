#include "TextRenderer.hpp"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <spdlog/spdlog.h>

using namespace sdl::core;

TextRenderer::TextRenderer(SDL_Renderer* renderer, const std::filesystem::path& fontPath, float fontSize)
  : m_pRenderer{renderer},
    m_pFont{TTF_OpenFont(fontPath.string().c_str(), fontSize)}
{
  if (!m_pFont)
    SPDLOG_ERROR("Failed to load font '{}': {}", fontPath.filename().string().c_str(), SDL_GetError());
}

TextRenderer::~TextRenderer()
{
  if (m_pFont)
  {
    TTF_CloseFont(m_pFont);
    m_pFont = nullptr;
  }
}

void TextRenderer::renderText(const std::string& text, int x, int y, const SDL_Color& color) const
{
  if (!m_pFont || !m_pRenderer || text.empty())
    return;

  auto* pSurface = TTF_RenderText_Solid(m_pFont, text.c_str(), text.length(), color);
  if (!pSurface)
  {
    SPDLOG_ERROR("TTF_RenderText_Solid failed for '{}': {}", text, SDL_GetError());
    return;
  }

  auto* pTexture = SDL_CreateTextureFromSurface(m_pRenderer, pSurface);
  if (!pTexture)
  {
    SPDLOG_ERROR("SDL_CreateTextureFromSurface failed: {}", SDL_GetError());
    SDL_DestroySurface(pSurface);
    return;
  }

  auto w = 0.f;
  auto h = 0.f;
  SDL_GetTextureSize(pTexture, &w, &h);
  const auto dest = SDL_FRect{.x = static_cast<float>(x), .y = static_cast<float>(y), .w = w, .h = h};

  SDL_RenderTexture(m_pRenderer, pTexture, nullptr, &dest);
  SDL_DestroyTexture(pTexture);
  SDL_DestroySurface(pSurface);
}

bool TextRenderer::valid() const
{
  return m_pFont != nullptr;
}