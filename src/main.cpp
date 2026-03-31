#include <App/Application.hpp>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

int main()
{
  auto logger = spdlog::stdout_color_mt("console");
  logger->set_pattern("[%H:%M:%S.%e] [%^%l%$] [%s:%#] [%!] %v");
  spdlog::set_default_logger(logger);
  spdlog::set_level(spdlog::level::trace);

  auto app = app::Application{};
  return app.exec();
}
