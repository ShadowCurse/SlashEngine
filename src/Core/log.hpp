#ifndef SLASHENGINE_CORE_LOG_H_
#define SLASHENGINE_CORE_LOG_H_

#include "core.hpp"
#include "spdlog/spdlog.h"

namespace slash {

class Slash_API Log {
private:
  Log() = default;

public:
  static void Init();

  inline static Shared<spdlog::logger> &GetEngineLogger() {
    return engineLogger;
  }
  inline static Shared<spdlog::logger> &GetApplicationLogger() {
    return applicationLogger;
  }

private:
  static Shared<spdlog::logger> engineLogger;
  static Shared<spdlog::logger> applicationLogger;
};
} // namespace slash

#define SL_CORE_ERROR(...) ::slash::Log::GetEngineLogger()->error(__VA_ARGS__)
#define SL_CORE_INFO(...) ::slash::Log::GetEngineLogger()->info(__VA_ARGS__)
#define SL_CORE_WARN(...) ::slash::Log::GetEngineLogger()->warn(__VA_ARGS__)
#define SL_CORE_TRACE(...) ::slash::Log::GetEngineLogger()->trace(__VA_ARGS__)
#define SL_CORE_FATAL(...) ::slash::Log::GetEngineLogger()->fatal(__VA_ARGS__)

#define SL_CORE_LOG_FUNC(className)                                            \
  ::slash::Log::GetEngineLogger()->trace(#className)

#define SL_ERROR(...) ::slash::Log::GetApplicationLogger()->error(__VA_ARGS__)
#define SL_INFO(...) ::slash::Log::GetApplicationLogger()->info(__VA_ARGS__)
#define SL_WARN(...) ::slash::Log::GetApplicationLogger()->warn(__VA_ARGS__)
#define SL_TRACE(...) ::slash::Log::GetApplicationLogger()->trace(__VA_ARGS__)
#define SL_FATAL(...) ::slash::Log::GetApplicationLogger()->fatal(__VA_ARGS__)

#endif // SLASHENGINE_CORE_LOG_H_