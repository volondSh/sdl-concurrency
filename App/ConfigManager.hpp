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
    void resetToDefaults();

    [[nodiscard]] int windowWidth() const;
    [[nodiscard]] int windowHeight() const;
    [[nodiscard]] bool windowResizable() const;
    [[nodiscard]] bool windowFullscreen() const;
    [[nodiscard]] std::string loggingLevel() const;

    void setWindowWidth(int width);
    void setWindowHeight(int height);
    void setWindowFullscreen(bool fullscreen);

    [[nodiscard]] int totalStars() const;
    [[nodiscard]] int movingStars() const;
    [[nodiscard]] int starMinBrightness() const;
    [[nodiscard]] int starMaxBrightness() const;
    [[nodiscard]] float starMinSpeed() const;
    [[nodiscard]] float starMaxSpeed() const;
    [[nodiscard]] float worldWidth() const;
    [[nodiscard]] float worldHeight() const;
    [[nodiscard]] float cameraPanSpeed() const;
    [[nodiscard]] float cameraZoomSpeed() const;
    [[nodiscard]] float cameraZoomMin() const;
    [[nodiscard]] float cameraZoomMax() const;

    void setTotalStars(int count);
    void setMovingStars(int count);
    void setTwinklingStars(int count);
    void setWorldWidth(float width);
    void setWorldHeight(float height);
    void setCameraPanSpeed(float speed);
    void setCameraZoomSpeed(float speed);
    void setCameraZoomMin(float zoom);
    void setCameraZoomMax(float zoom);

    [[nodiscard]] int twinklingStars() const;
    [[nodiscard]] float twinkleMinFreq() const;
    [[nodiscard]] float twinkleMaxFreq() const;
    [[nodiscard]] float twinkleMinAmp() const;
    [[nodiscard]] float twinkleMaxAmp() const;

    [[nodiscard]] int shortcutMoveUp() const;
    [[nodiscard]] int shortcutMoveDown() const;
    [[nodiscard]] int shortcutMoveLeft() const;
    [[nodiscard]] int shortcutMoveRight() const;
    [[nodiscard]] int shortcutZoomIn() const;
    [[nodiscard]] int shortcutZoomOut() const;
    [[nodiscard]] int shortcutToggleOverlay() const;
    [[nodiscard]] int shortcutToggleMenu() const;
    [[nodiscard]] int shortcutToggleFullscreen() const;

    void setShortcutMoveUp(int scancode);
    void setShortcutMoveDown(int scancode);
    void setShortcutMoveLeft(int scancode);
    void setShortcutMoveRight(int scancode);
    void setShortcutZoomIn(int scancode);
    void setShortcutZoomOut(int scancode);
    void setShortcutToggleOverlay(int scancode);
    void setShortcutToggleMenu(int scancode);
    void setShortcutToggleFullscreen(int scancode);

  private:
    std::filesystem::path m_configPath;
    nlohmann::json m_config;
  };
}