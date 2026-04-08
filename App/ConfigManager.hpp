#pragma once

#include <nlohmann/json.hpp>

#include <filesystem>
#include <string>

namespace app
{
  class ConfigManager
  {
  public:
    explicit ConfigManager(const std::filesystem::path& configPath);

    bool load();
    [[nodiscard]] bool save() const;

    [[nodiscard]] int windowWidth() const;
    [[nodiscard]] int windowHeight() const;
    [[nodiscard]] bool windowResizable() const;
    [[nodiscard]] bool windowFullscreen() const;
    [[nodiscard]] std::string loggingLevel() const;

    void setWindowWidth(int width);
    void setWindowHeight(int height);
    void setWindowFullscreen(bool fullscreen);

  private:
    std::filesystem::path m_configPath;
    nlohmann::json m_config;
  };
}