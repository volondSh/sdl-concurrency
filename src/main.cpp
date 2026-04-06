#include <App/Logger.hpp>
#include <App/Application.hpp>
#include <App/ConfigManager.hpp>

#include <filesystem>
#include <memory>

int main(int argc, char* argv[])
{
  auto pConfigManager =
      std::make_unique<app::ConfigManager>(std::filesystem::canonical(argv[0]).parent_path() / "app_config.json");
  auto pLogger = app::createLogger(app::convertLogLevel(pConfigManager->loggingLevel()));

  auto app = app::Application{std::move(pConfigManager)};
  return app.exec();
}
