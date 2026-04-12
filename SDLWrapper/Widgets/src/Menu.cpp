#include "Menu.hpp"

#include <App/ConfigManager.hpp>

#include <SDL3/SDL.h>

#include <algorithm>

#include <imgui.h>
#include <imgui_impl_sdl3.h>

using namespace sdl::widgets;

Menu::Menu(app::ConfigManager& configManager) : m_config{configManager}
{
  loadPendingFromConfig();
}

bool Menu::isOpen() const
{
  return m_open;
}

ShortcutField Menu::capturingField() const
{
  return m_captureField;
}

void Menu::startCapture(ShortcutField field)
{
  m_captureField = field;
}

void Menu::cancelCapture()
{
  m_captureField = ShortcutField::None;
}

void Menu::setOnSave(std::function<void()> callback)
{
  m_onSave = std::move(callback);
}

const SettingsPending& Menu::pending() const
{
  return m_pending;
}

void Menu::loadPendingFromConfig()
{
  m_pending.totalStars      = m_config.totalStars();
  m_pending.movingStars     = m_config.movingStars();
  m_pending.twinklingStars  = m_config.twinklingStars();
  m_pending.worldWidth      = m_config.worldWidth();
  m_pending.worldHeight     = m_config.worldHeight();
  m_pending.cameraPanSpeed  = m_config.cameraPanSpeed();
  m_pending.cameraZoomSpeed = m_config.cameraZoomSpeed();
  m_pending.cameraZoomMin   = m_config.cameraZoomMin();
  m_pending.cameraZoomMax   = m_config.cameraZoomMax();
}

void Menu::applyCapture(int scancode)
{
  switch (m_captureField)
  {
  case ShortcutField::MoveUp:
    m_config.setShortcutMoveUp(scancode);
    break;
  case ShortcutField::MoveDown:
    m_config.setShortcutMoveDown(scancode);
    break;
  case ShortcutField::MoveLeft:
    m_config.setShortcutMoveLeft(scancode);
    break;
  case ShortcutField::MoveRight:
    m_config.setShortcutMoveRight(scancode);
    break;
  case ShortcutField::ZoomIn:
    m_config.setShortcutZoomIn(scancode);
    break;
  case ShortcutField::ZoomOut:
    m_config.setShortcutZoomOut(scancode);
    break;
  case ShortcutField::ToggleOverlay:
    m_config.setShortcutToggleOverlay(scancode);
    break;
  case ShortcutField::ToggleMenu:
    m_config.setShortcutToggleMenu(scancode);
    break;
  case ShortcutField::ToggleFullscreen:
    m_config.setShortcutToggleFullscreen(scancode);
    break;
  default:
    break;
  }
  m_captureField = ShortcutField::None;
}

void Menu::render()
{
  if (!m_open)
    return;

  const auto& viewport = *ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport.GetCenter(), ImGuiCond_Always, {0.5f, 0.5f});

  ImGui::Begin(
      "Menu",
      nullptr,
      ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

  switch (m_state)
  {
  case MenuState::Main:
    renderMain();
    break;
  case MenuState::Settings:
    renderSettings();
    break;
  case MenuState::Controls:
    renderControls();
    break;
  }

  ImGui::End();
}

void Menu::renderMain()
{
  ImGui::SeparatorText("Main Menu");

  if (ImGui::Button("Settings", {200, 40}))
    m_state = MenuState::Settings;

  if (ImGui::Button("Controls", {200, 40}))
    m_state = MenuState::Controls;

  ImGui::Spacing();

  if (ImGui::Button("Exit", {200, 40}))
  {
    auto quitEvent = SDL_Event{};
    quitEvent.type = SDL_EVENT_QUIT;
    SDL_PushEvent(&quitEvent);
  }

  ImGui::Spacing();

  if (ImGui::Button("Back", {200, 40}))
    toggle();
}

void Menu::renderSettings()
{
  ImGui::SeparatorText("Settings");

  if (ImGui::SliderInt("Total Stars", &m_pending.totalStars, 100, 1000000))
  {
    m_pending.movingStars    = std::min(m_pending.movingStars, m_pending.totalStars);
    m_pending.twinklingStars = std::min(m_pending.twinklingStars, m_pending.totalStars);
  }

  if (ImGui::SliderInt("Moving Stars", &m_pending.movingStars, 0, m_pending.totalStars))
    m_pending.movingStars = std::clamp(m_pending.movingStars, 0, m_pending.totalStars);

  if (ImGui::SliderInt("Twinkling Stars", &m_pending.twinklingStars, 0, m_pending.totalStars))
    m_pending.twinklingStars = std::clamp(m_pending.twinklingStars, 0, m_pending.totalStars);

  ImGui::Spacing();

  ImGui::SliderFloat("World Width", &m_pending.worldWidth, 500.f, 10000.f, "%.0f");
  ImGui::SliderFloat("World Height", &m_pending.worldHeight, 500.f, 10000.f, "%.0f");

  ImGui::Spacing();

  ImGui::SliderFloat("Camera Pan Speed", &m_pending.cameraPanSpeed, 50.f, 1000.f, "%.0f");
  ImGui::SliderFloat("Camera Zoom Speed", &m_pending.cameraZoomSpeed, 1.01f, 1.5f, "%.3f");
  ImGui::SliderFloat("Camera Zoom Min", &m_pending.cameraZoomMin, 0.01f, 1.f, "%.2f");
  ImGui::SliderFloat("Camera Zoom Max", &m_pending.cameraZoomMax, 1.f, 10.f, "%.1f");

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  if (ImGui::Button("Save & Apply", {200, 40}))
  {
    m_config.setTotalStars(m_pending.totalStars);
    m_config.setMovingStars(m_pending.movingStars);
    m_config.setTwinklingStars(m_pending.twinklingStars);
    m_config.setWorldWidth(m_pending.worldWidth);
    m_config.setWorldHeight(m_pending.worldHeight);
    m_config.setCameraPanSpeed(m_pending.cameraPanSpeed);
    m_config.setCameraZoomSpeed(m_pending.cameraZoomSpeed);
    m_config.setCameraZoomMin(m_pending.cameraZoomMin);
    m_config.setCameraZoomMax(m_pending.cameraZoomMax);

    if (m_onSave)
      m_onSave();
  }

  ImGui::SameLine();

  if (ImGui::Button("Reset to Defaults", {200, 40}))
  {
    m_config.resetToDefaults();
    loadPendingFromConfig();
  }

  ImGui::Spacing();

  if (ImGui::Button("Back", {200, 40}))
  {
    m_state = MenuState::Main;
    loadPendingFromConfig();
  }
}

void Menu::renderControls()
{
  ImGui::SeparatorText("Controls");

  if (m_captureField != ShortcutField::None)
  {
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Press any key... (Escape to cancel)");
    ImGui::Spacing();
  }
  else
  {
    ImGui::Text("Click a button then press a key to rebind.");
    ImGui::Spacing();
  }

  renderShortcutButton("Move Up", ShortcutField::MoveUp);
  renderShortcutButton("Move Down", ShortcutField::MoveDown);
  renderShortcutButton("Move Left", ShortcutField::MoveLeft);
  renderShortcutButton("Move Right", ShortcutField::MoveRight);

  ImGui::Spacing();

  renderShortcutButton("Zoom In", ShortcutField::ZoomIn);
  renderShortcutButton("Zoom Out", ShortcutField::ZoomOut);

  ImGui::Spacing();

  renderShortcutButton("Toggle Overlay", ShortcutField::ToggleOverlay);
  renderShortcutButton("Toggle Fullscreen", ShortcutField::ToggleFullscreen);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  if (ImGui::Button("Back", {200, 40}))
    m_state = MenuState::Main;
}

void Menu::renderShortcutButton(std::string_view label, ShortcutField field)
{
  auto scancode = 0;
  switch (field)
  {
  case ShortcutField::MoveUp:
    scancode = m_config.shortcutMoveUp();
    break;
  case ShortcutField::MoveDown:
    scancode = m_config.shortcutMoveDown();
    break;
  case ShortcutField::MoveLeft:
    scancode = m_config.shortcutMoveLeft();
    break;
  case ShortcutField::MoveRight:
    scancode = m_config.shortcutMoveRight();
    break;
  case ShortcutField::ZoomIn:
    scancode = m_config.shortcutZoomIn();
    break;
  case ShortcutField::ZoomOut:
    scancode = m_config.shortcutZoomOut();
    break;
  case ShortcutField::ToggleOverlay:
    scancode = m_config.shortcutToggleOverlay();
    break;
  case ShortcutField::ToggleMenu:
    scancode = m_config.shortcutToggleMenu();
    break;
  case ShortcutField::ToggleFullscreen:
    scancode = m_config.shortcutToggleFullscreen();
    break;
  default:
    break;
  }

  const auto* name     = SDL_GetScancodeName(static_cast<SDL_Scancode>(scancode));
  const auto capturing = (m_captureField != ShortcutField::None);

  if (capturing)
    ImGui::BeginDisabled();

  if (ImGui::Button(label.data(), {150, 30}))
    m_captureField = field;

  if (capturing)
    ImGui::EndDisabled();

  ImGui::SameLine();

  if (m_captureField == field)
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "[waiting for key]");
  else
    ImGui::Text("%s", name && name[0] ? name : "Unknown");
}

void Menu::toggle()
{
  m_open = !m_open;
  if (m_open)
  {
    m_state = MenuState::Main;
    loadPendingFromConfig();
  }
  else
    m_captureField = ShortcutField::None;
}
