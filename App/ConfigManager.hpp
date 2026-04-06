#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>
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

  private:
    std::filesystem::path m_configPath;
    nlohmann::json m_config;
  };
}