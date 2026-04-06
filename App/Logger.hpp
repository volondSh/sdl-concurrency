#pragma once

#include <spdlog/spdlog.h>

namespace app
{
  std::shared_ptr<spdlog::logger> createLogger(spdlog::level::level_enum);
  spdlog::level::level_enum convertLogLevel(const std::string&);
}