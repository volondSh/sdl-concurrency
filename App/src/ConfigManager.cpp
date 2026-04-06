#include "ConfigManager.hpp"

#include <spdlog/spdlog.h>

#include <fstream>

using namespace app;

namespace
{
  nlohmann::json defaultConfig()
  {
    auto defaults                    = nlohmann::json::object();
    defaults["window"]["width"]      = 1280;
    defaults["window"]["height"]     = 720;
    defaults["window"]["resizable"]  = true;
    defaults["window"]["fullscreen"] = false;
    defaults["logging"]["level"]     = "off";
    return defaults;
  }

  bool validate(const nlohmann::json& config)
  {
    if (!config.is_object())
      return false;

    if (!config.contains("window") || !config["window"].is_object())
      return false;

    const auto& window = config["window"];
    if (!window.contains("width") || !window["width"].is_number())
      return false;
    if (!window.contains("height") || !window["height"].is_number())
      return false;
    if (!window.contains("resizable") || !window["resizable"].is_boolean())
      return false;
    if (!window.contains("fullscreen") || !window["fullscreen"].is_boolean())
      return false;

    if (!config.contains("logging") || !config["logging"].is_object())
      return false;

    const auto& logging = config["logging"];
    return logging.contains("level") && logging["level"].is_string();
  }
}

ConfigManager::ConfigManager(const std::filesystem::path& configPath)
  : m_configPath{configPath},
    m_config(defaultConfig())
{
  load();
}

bool ConfigManager::load()
{
  if (!std::filesystem::exists(m_configPath))
  {
    SPDLOG_WARN("Config file '{}' not found, using defaults.", m_configPath.string());
    return false;
  }

  auto file = std::ifstream{m_configPath};
  if (!file.is_open())
  {
    SPDLOG_WARN("Config file '{}' cannot be opened, using defaults.", m_configPath.string());
    return false;
  }

  try
  {
    auto loaded = nlohmann::json::parse(file);
    if (!validate(loaded))
    {
      SPDLOG_WARN("Config file '{}' is not a valid JSON object, using defaults.", m_configPath.string());
      return false;
    }

    m_config.merge_patch(loaded);
    SPDLOG_INFO("Configuration loaded from '{}'", m_configPath.string());
    return true;
  }
  catch (const std::exception& e)
  {
    SPDLOG_ERROR("Failed to parse config file '{}': {}", m_configPath.string(), e.what());
    return false;
  }
}

bool ConfigManager::save() const
{
  try
  {
    std::filesystem::create_directories(m_configPath.parent_path());

    std::ofstream file(m_configPath);
    if (!file.is_open())
    {
      SPDLOG_ERROR("Failed to open config file '{}' for writing.", m_configPath.string());
      return false;
    }

    file << m_config.dump(2);
    file.close();

    SPDLOG_INFO("Configuration saved to '{}'", m_configPath.string());
    return true;
  }
  catch (const std::exception& e)
  {
    SPDLOG_ERROR("Failed to save config file '{}': {}", m_configPath.string(), e.what());
    return false;
  }
}

int ConfigManager::windowWidth() const
{
  return m_config["window"]["width"].get<int>();
}

int ConfigManager::windowHeight() const
{
  return m_config["window"]["height"].get<int>();
}

bool ConfigManager::windowResizable() const
{
  return m_config["window"]["resizable"].get<bool>();
}

bool ConfigManager::windowFullscreen() const
{
  return m_config["window"]["fullscreen"].get<bool>();
}

std::string ConfigManager::loggingLevel() const
{
  return m_config["logging"]["level"].get<std::string>();
}