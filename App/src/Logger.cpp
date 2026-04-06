#include "App/Logger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> app::createLogger(spdlog::level::level_enum level)
{
  auto pLogger = spdlog::stdout_color_mt("console");
  pLogger->set_pattern("[%H:%M:%S.%e] [%^%l%$] [%s:%#] [%!] %v");
  spdlog::set_default_logger(pLogger);
  spdlog::set_level(level);

  return pLogger;
}

spdlog::level::level_enum app::convertLogLevel(const std::string& level)
{
  if (level == "trace")
    return spdlog::level::trace;
  if (level == "debug")
    return spdlog::level::debug;
  if (level == "info")
    return spdlog::level::info;
  if (level == "warn")
    return spdlog::level::warn;
  if (level == "error")
    return spdlog::level::err;
  if (level == "critical")
    return spdlog::level::critical;
  return spdlog::level::off;
}