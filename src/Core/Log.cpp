#include "Log.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Slash
{
    Shared<spdlog::logger> Log::engineLogger;
    Shared<spdlog::logger> Log::applicationLogger;

    void Log::Init()
    {
        spdlog::set_pattern("%^[%T] %n: %v%$");
        engineLogger = spdlog::stdout_color_mt("SLASH");
        engineLogger->set_level(spdlog::level::trace);

        applicationLogger = spdlog::stdout_color_mt("APP");
        applicationLogger->set_level(spdlog::level::trace);
    }
} // namespace Slash