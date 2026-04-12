#include "ImGuiRenderer.hpp"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include <spdlog/spdlog.h>

using namespace sdl::core;

ImGuiRenderer::ImGuiRenderer(SDL_Window* pWindow, SDL_Renderer* renderer) : m_pWindow{pWindow}, m_pRenderer{renderer}
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  if (!ImGui_ImplSDL3_InitForSDLRenderer(pWindow, renderer))
    SPDLOG_ERROR("Failed to initialize ImGui SDL3 backend");
  if (!ImGui_ImplSDLRenderer3_Init(renderer))
    SPDLOG_ERROR("Failed to initialize ImGui SDLRenderer3 backend");
}

ImGuiRenderer::~ImGuiRenderer()
{
  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();
}

void ImGuiRenderer::newFrame()
{
  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();
}

void ImGuiRenderer::render()
{
  ImGui::Render();
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_pRenderer);
}

SDL_Window* ImGuiRenderer::window() const
{
  return m_pWindow;
}

SDL_Renderer* ImGuiRenderer::renderer() const
{
  return m_pRenderer;
}