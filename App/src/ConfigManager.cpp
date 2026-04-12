#include "ConfigManager.hpp"

#include <spdlog/spdlog.h>

#include <fstream>

using namespace app;

namespace
{
  nlohmann::json defaultConfig()
  {
    auto defaults                               = nlohmann::json::object();
    defaults["window"]["width"]                 = 1280;
    defaults["window"]["height"]                = 720;
    defaults["window"]["resizable"]             = true;
    defaults["window"]["fullscreen"]            = false;
    defaults["logging"]["level"]                = "off";
    defaults["scene"]["stars"]["total"]         = 2500;
    defaults["scene"]["stars"]["moving"]        = 200;
    defaults["scene"]["stars"]["minBrightness"] = 100;
    defaults["scene"]["stars"]["maxBrightness"] = 255;
    defaults["scene"]["stars"]["minSpeed"]      = 5.f;
    defaults["scene"]["stars"]["maxSpeed"]      = 30.f;
    defaults["scene"]["worldSize"]["width"]     = 4000.f;
    defaults["scene"]["worldSize"]["height"]    = 4000.f;
    defaults["scene"]["camera"]["panSpeed"]     = 300.f;
    defaults["scene"]["camera"]["zoomSpeed"]    = 1.05f;
    defaults["scene"]["camera"]["zoomMin"]      = 0.1f;
    defaults["scene"]["camera"]["zoomMax"]      = 5.f;
    defaults["scene"]["twinkle"]["stars"]       = 500;
    defaults["scene"]["twinkle"]["minFreq"]     = 0.5f;
    defaults["scene"]["twinkle"]["maxFreq"]     = 3.0f;
    defaults["scene"]["twinkle"]["minAmp"]      = 0.3f;
    defaults["scene"]["twinkle"]["maxAmp"]      = 0.8f;
    defaults["shortcuts"]["moveUp"]             = 26; // W
    defaults["shortcuts"]["moveDown"]           = 22; // S
    defaults["shortcuts"]["moveLeft"]           = 4;  // A
    defaults["shortcuts"]["moveRight"]          = 7;  // D
    defaults["shortcuts"]["zoomIn"]             = 8;  // E
    defaults["shortcuts"]["zoomOut"]            = 20; // Q
    defaults["shortcuts"]["toggleOverlay"]      = 53; // `
    defaults["shortcuts"]["toggleMenu"]         = 27; // P
    defaults["shortcuts"]["toggleFullscreen"]   = 68; // F11
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
    if (!logging.contains("level") || !logging["level"].is_string())
      return false;

    if (config.contains("scene"))
    {
      const auto& scene = config["scene"];
      if (scene.contains("stars"))
      {
        const auto& stars = scene["stars"];
        if (stars.contains("total") && !stars["total"].is_number())
          return false;
        if (stars.contains("moving") && !stars["moving"].is_number())
          return false;
        if (stars.contains("minBrightness") && !stars["minBrightness"].is_number())
          return false;
        if (stars.contains("maxBrightness") && !stars["maxBrightness"].is_number())
          return false;
        if (stars.contains("minSpeed") && !stars["minSpeed"].is_number())
          return false;
        if (stars.contains("maxSpeed") && !stars["maxSpeed"].is_number())
          return false;
      }
      if (scene.contains("worldSize"))
      {
        const auto& worldSize = scene["worldSize"];
        if (worldSize.contains("width") && !worldSize["width"].is_number())
          return false;
        if (worldSize.contains("height") && !worldSize["height"].is_number())
          return false;
      }
      if (scene.contains("camera"))
      {
        const auto& camera = scene["camera"];
        if (camera.contains("panSpeed") && !camera["panSpeed"].is_number())
          return false;
        if (camera.contains("zoomSpeed") && !camera["zoomSpeed"].is_number())
          return false;
        if (camera.contains("zoomMin") && !camera["zoomMin"].is_number())
          return false;
        if (camera.contains("zoomMax") && !camera["zoomMax"].is_number())
          return false;
      }
      if (scene.contains("twinkle"))
      {
        const auto& twinkle = scene["twinkle"];
        if (twinkle.contains("stars") && !twinkle["stars"].is_number())
          return false;
        if (twinkle.contains("minFreq") && !twinkle["minFreq"].is_number())
          return false;
        if (twinkle.contains("maxFreq") && !twinkle["maxFreq"].is_number())
          return false;
        if (twinkle.contains("minAmp") && !twinkle["minAmp"].is_number())
          return false;
        if (twinkle.contains("maxAmp") && !twinkle["maxAmp"].is_number())
          return false;
      }
      if (config.contains("shortcuts"))
      {
        const auto& shortcuts           = config["shortcuts"];
        static const std::string keys[] = {
            "moveUp",
            "moveDown",
            "moveLeft",
            "moveRight",
            "zoomIn",
            "zoomOut",
            "toggleOverlay",
            "toggleMenu",
            "toggleFullscreen"};
        for (const auto& key : keys)
        {
          if (shortcuts.contains(key) && !shortcuts[key].is_number())
            return false;
        }
      }
    }

    return true;
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

void ConfigManager::resetToDefaults()
{
  m_config = defaultConfig();
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

void ConfigManager::setWindowWidth(int width)
{
  m_config["window"]["width"] = width;
}

void ConfigManager::setWindowHeight(int height)
{
  m_config["window"]["height"] = height;
}

void ConfigManager::setWindowFullscreen(bool fullscreen)
{
  m_config["window"]["fullscreen"] = fullscreen;
}

int ConfigManager::totalStars() const
{
  return m_config["scene"]["stars"]["total"].get<int>();
}

int ConfigManager::movingStars() const
{
  return m_config["scene"]["stars"]["moving"].get<int>();
}

int ConfigManager::starMinBrightness() const
{
  return m_config["scene"]["stars"]["minBrightness"].get<int>();
}

int ConfigManager::starMaxBrightness() const
{
  return m_config["scene"]["stars"]["maxBrightness"].get<int>();
}

float ConfigManager::starMinSpeed() const
{
  return m_config["scene"]["stars"]["minSpeed"].get<float>();
}

float ConfigManager::starMaxSpeed() const
{
  return m_config["scene"]["stars"]["maxSpeed"].get<float>();
}

float ConfigManager::worldWidth() const
{
  return m_config["scene"]["worldSize"]["width"].get<float>();
}

float ConfigManager::worldHeight() const
{
  return m_config["scene"]["worldSize"]["height"].get<float>();
}

void ConfigManager::setTotalStars(int count)
{
  m_config["scene"]["stars"]["total"] = count;
}

void ConfigManager::setMovingStars(int count)
{
  m_config["scene"]["stars"]["moving"] = count;
}

void ConfigManager::setTwinklingStars(int count)
{
  m_config["scene"]["twinkle"]["stars"] = count;
}

void ConfigManager::setWorldWidth(float width)
{
  m_config["scene"]["worldSize"]["width"] = width;
}

void ConfigManager::setWorldHeight(float height)
{
  m_config["scene"]["worldSize"]["height"] = height;
}

void ConfigManager::setCameraPanSpeed(float speed)
{
  m_config["scene"]["camera"]["panSpeed"] = speed;
}

void ConfigManager::setCameraZoomSpeed(float speed)
{
  m_config["scene"]["camera"]["zoomSpeed"] = speed;
}

void ConfigManager::setCameraZoomMin(float zoom)
{
  m_config["scene"]["camera"]["zoomMin"] = zoom;
}

void ConfigManager::setCameraZoomMax(float zoom)
{
  m_config["scene"]["camera"]["zoomMax"] = zoom;
}

float ConfigManager::cameraPanSpeed() const
{
  return m_config["scene"]["camera"]["panSpeed"].get<float>();
}

float ConfigManager::cameraZoomSpeed() const
{
  return m_config["scene"]["camera"]["zoomSpeed"].get<float>();
}

float ConfigManager::cameraZoomMin() const
{
  return m_config["scene"]["camera"]["zoomMin"].get<float>();
}

float ConfigManager::cameraZoomMax() const
{
  return m_config["scene"]["camera"]["zoomMax"].get<float>();
}

int ConfigManager::twinklingStars() const
{
  return m_config["scene"]["twinkle"]["stars"].get<int>();
}

float ConfigManager::twinkleMinFreq() const
{
  return m_config["scene"]["twinkle"]["minFreq"].get<float>();
}

float ConfigManager::twinkleMaxFreq() const
{
  return m_config["scene"]["twinkle"]["maxFreq"].get<float>();
}

float ConfigManager::twinkleMinAmp() const
{
  return m_config["scene"]["twinkle"]["minAmp"].get<float>();
}

float ConfigManager::twinkleMaxAmp() const
{
  return m_config["scene"]["twinkle"]["maxAmp"].get<float>();
}

int ConfigManager::shortcutMoveUp() const
{
  return m_config["shortcuts"]["moveUp"].get<int>();
}

int ConfigManager::shortcutMoveDown() const
{
  return m_config["shortcuts"]["moveDown"].get<int>();
}

int ConfigManager::shortcutMoveLeft() const
{
  return m_config["shortcuts"]["moveLeft"].get<int>();
}

int ConfigManager::shortcutMoveRight() const
{
  return m_config["shortcuts"]["moveRight"].get<int>();
}

int ConfigManager::shortcutZoomIn() const
{
  return m_config["shortcuts"]["zoomIn"].get<int>();
}

int ConfigManager::shortcutZoomOut() const
{
  return m_config["shortcuts"]["zoomOut"].get<int>();
}

int ConfigManager::shortcutToggleOverlay() const
{
  return m_config["shortcuts"]["toggleOverlay"].get<int>();
}

int ConfigManager::shortcutToggleMenu() const
{
  return m_config["shortcuts"]["toggleMenu"].get<int>();
}

int ConfigManager::shortcutToggleFullscreen() const
{
  return m_config["shortcuts"]["toggleFullscreen"].get<int>();
}

void ConfigManager::setShortcutMoveUp(int scancode)
{
  m_config["shortcuts"]["moveUp"] = scancode;
}

void ConfigManager::setShortcutMoveDown(int scancode)
{
  m_config["shortcuts"]["moveDown"] = scancode;
}

void ConfigManager::setShortcutMoveLeft(int scancode)
{
  m_config["shortcuts"]["moveLeft"] = scancode;
}

void ConfigManager::setShortcutMoveRight(int scancode)
{
  m_config["shortcuts"]["moveRight"] = scancode;
}

void ConfigManager::setShortcutZoomIn(int scancode)
{
  m_config["shortcuts"]["zoomIn"] = scancode;
}

void ConfigManager::setShortcutZoomOut(int scancode)
{
  m_config["shortcuts"]["zoomOut"] = scancode;
}

void ConfigManager::setShortcutToggleOverlay(int scancode)
{
  m_config["shortcuts"]["toggleOverlay"] = scancode;
}

void ConfigManager::setShortcutToggleMenu(int scancode)
{
  m_config["shortcuts"]["toggleMenu"] = scancode;
}

void ConfigManager::setShortcutToggleFullscreen(int scancode)
{
  m_config["shortcuts"]["toggleFullscreen"] = scancode;
}
