#ifndef SL_LOG
#define SL_LOG

#include "Core.hpp"
#include "spdlog/spdlog.h"

namespace Slash
{
    class Slash_API Log
    {
    private:
        Log() {}
    public:
        static void Init();

        inline static Shared<spdlog::logger>& GetEngineLogger() {
            return engineLogger;
        }
        inline static Shared<spdlog::logger>& GetApplicationLogger() {
            return applicationLogger;
        }
    private:
        static Shared<spdlog::logger> engineLogger;
        static Shared<spdlog::logger> applicationLogger;
    };
} // namespace Slash

#define SL_CORE_ERROR(...) ::Slash::Log::GetEngineLogger()->error(__VA_ARGS__)
#define SL_CORE_INFO(...)  ::Slash::Log::GetEngineLogger()->info(__VA_ARGS__)
#define SL_CORE_WARN(...)  ::Slash::Log::GetEngineLogger()->warn(__VA_ARGS__)
#define SL_CORE_TRACE(...) ::Slash::Log::GetEngineLogger()->trace(__VA_ARGS__)
#define SL_CORE_FATAL(...) ::Slash::Log::GetEngineLogger()->fatal(__VA_ARGS__)

#define SL_ERROR(...) ::Slash::Log::GetApplicationLogger()->error(__VA_ARGS__)
#define SL_INFO(...)  ::Slash::Log::GetApplicationLogger()->info(__VA_ARGS__)
#define SL_WARN(...)  ::Slash::Log::GetApplicationLogger()->warn(__VA_ARGS__)
#define SL_TRACE(...) ::Slash::Log::GetApplicationLogger()->trace(__VA_ARGS__)
#define SL_FATAL(...) ::Slash::Log::GetApplicationLogger()->fatal(__VA_ARGS__)

#endif