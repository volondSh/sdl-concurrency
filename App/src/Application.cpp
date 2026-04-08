#include "Application.hpp"

#include "ConfigManager.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

#include <spdlog/spdlog.h>

using namespace sdl::core;
using namespace sdl::widgets;
using namespace app;

Application::Application(ConfigManager& configManager)
  : m_context{SDL_INIT_EVENTS | SDL_INIT_VIDEO},
    m_mainWindow{
        "sdl-concurrency",
        configManager.windowWidth(),
        configManager.windowHeight(),
        convertWindowSettingsToFlags(configManager.windowResizable(), configManager.windowFullscreen())},
    m_eventLoop{m_mainWindow},
    m_configManager{configManager}
{
}

Application::~Application()
{
  m_configManager.setWindowWidth(m_mainWindow.restoreWidth());
  m_configManager.setWindowHeight(m_mainWindow.restoreHeight());
  m_configManager.setWindowFullscreen(m_mainWindow.fullscreen());

  if (!m_configManager.save())
    SPDLOG_ERROR("Unable to save config");
}

bool Application::ready() const noexcept
{
  return m_context.initialized() && m_mainWindow.valid();
}

int Application::exec()
{
  if (!m_context.initialized())
  {
    SPDLOG_CRITICAL("Unable to start SDL3: {}", m_context.init_error());
    return 1;
  }
  if (!m_mainWindow.valid())
  {
    SPDLOG_CRITICAL("Unable to create window");
    return 1;
  }

  SPDLOG_INFO("SDL window created. Close window or press Escape to exit.");
  m_eventLoop.run();
  return 0;
}
