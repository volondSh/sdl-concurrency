#include <App/Application.hpp>
#include <App/ConfigManager.hpp>
#include <App/Logger.hpp>

#include <filesystem>

#ifdef _WIN32
  #include <windows.h>
#endif

int main(int argc, char* argv[])
{
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
#endif

  auto configManager = app::ConfigManager{std::filesystem::canonical(argv[0]).parent_path() / "app_config.json"};
  auto pLogger       = app::createLogger(app::convertLogLevel(configManager.loggingLevel()));

  auto app = app::Application{configManager};
  return app.exec();
}
