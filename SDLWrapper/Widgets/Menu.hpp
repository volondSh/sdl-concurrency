#pragma once

#include <functional>
#include <string_view>

struct SDL_Window;

namespace app
{
  class ConfigManager;
}

namespace sdl::widgets
{
  enum class MenuState
  {
    Main,
    Settings,
    Controls,
  };

  enum class ShortcutField
  {
    None,
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    ZoomIn,
    ZoomOut,
    ToggleOverlay,
    ToggleMenu,
    ToggleFullscreen,
  };

  struct SettingsPending
  {
    int totalStars        = 0;
    int movingStars       = 0;
    int twinklingStars    = 0;
    float worldWidth      = 0.f;
    float worldHeight     = 0.f;
    float cameraPanSpeed  = 0.f;
    float cameraZoomSpeed = 0.f;
    float cameraZoomMin   = 0.f;
    float cameraZoomMax   = 0.f;
  };

  class Menu final
  {
  public:
    explicit Menu(app::ConfigManager&);

    void render();
    void toggle();

    [[nodiscard]] bool isOpen() const;
    [[nodiscard]] ShortcutField capturingField() const;

    void startCapture(ShortcutField field);
    void cancelCapture();
    void applyCapture(int scancode);

    [[nodiscard]] const SettingsPending& pending() const;

    void loadPendingFromConfig();

    void setOnSave(std::function<void()> callback);

  private:
    void renderMain();
    void renderSettings();
    void renderControls();
    void renderShortcutButton(std::string_view label, ShortcutField field);

    app::ConfigManager& m_config;
    bool m_open                  = false;
    MenuState m_state            = MenuState::Main;
    ShortcutField m_captureField = ShortcutField::None;
    SettingsPending m_pending;
    std::function<void()> m_onSave;
  };
}
