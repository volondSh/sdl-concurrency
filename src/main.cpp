#include <App/Logger.hpp>
#include <App/Application.hpp>
#include <App/ConfigManager.hpp>

#include <filesystem>

int main(int argc, char* argv[])
{
  auto configManager = app::ConfigManager{std::filesystem::canonical(argv[0]).parent_path() / "app_config.json"};
  auto pLogger       = app::createLogger(app::convertLogLevel(configManager.loggingLevel()));

  auto app = app::Application{configManager};
  return app.exec();
}
