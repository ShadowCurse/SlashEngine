#ifndef SLASHENGINE_CORE_LOG_H_
#define SLASHENGINE_CORE_LOG_H_

#include "core.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace slash {

class Slash_API Log {
 private:
  Log() = default;

 public:
  static void Init() {
    spdlog::set_pattern("%^[%T] %n: %v%$");
    engine_logger_ = spdlog::stdout_color_mt("SLASH");
    engine_logger_->set_level(spdlog::level::trace);

    app_logger_ = spdlog::stdout_color_mt("APP");
    app_logger_->set_level(spdlog::level::trace);
  }

  static auto engine_logger() -> std::shared_ptr<spdlog::logger> & {
    return engine_logger_;
  }
  static auto app_logger() -> std::shared_ptr<spdlog::logger> & {
    return app_logger_;
  }

 private:
  static inline std::shared_ptr<spdlog::logger> engine_logger_;
  static inline std::shared_ptr<spdlog::logger> app_logger_;
};

} // namespace slash

#define SL_CORE_ERROR(...) ::slash::Log::engine_logger()->error(__VA_ARGS__)
#define SL_CORE_INFO(...) ::slash::Log::engine_logger()->info(__VA_ARGS__)
#define SL_CORE_WARN(...) ::slash::Log::engine_logger()->warn(__VA_ARGS__)
#define SL_CORE_TRACE(...) ::slash::Log::engine_logger()->trace(__VA_ARGS__)
#define SL_CORE_FATAL(...) ::slash::Log::engine_logger()->fatal(__VA_ARGS__)

#define SL_CORE_LOG_FUNC(className)                                            \
  ::slash::Log::engine_logger()->trace(#className)

#define SL_ERROR(...) ::slash::Log::app_logger()->error(__VA_ARGS__)
#define SL_INFO(...) ::slash::Log::app_logger()->info(__VA_ARGS__)
#define SL_WARN(...) ::slash::Log::app_logger()->warn(__VA_ARGS__)
#define SL_TRACE(...) ::slash::Log::app_logger()->trace(__VA_ARGS__)
#define SL_FATAL(...) ::slash::Log::app_logger()->fatal(__VA_ARGS__)

#endif // SLASHENGINE_CORE_LOG_H_