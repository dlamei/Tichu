#ifndef TICHU_LOGGING_H
#define TICHU_LOGGING_H

#include <spdlog/spdlog.h>

#define TRACE_LOG(...) ::spdlog::trace(__VA_ARGS__)
#define INFO_LOG(...) ::spdlog::info(__VA_ARGS__)
#define WARN_LOG(...) ::spdlog::warn(__VA_ARGS__)
#define ERROR_LOG(...) ::spdlog::error(__VA_ARGS__)

#if NDEBUG
#define ASSERT(x, ...)
#else
#define ASSERT(x, ...) { if(!x) {ERROR_LOG("Assertion Failed: {0}", __VA_ARGS__); assert(false); } }
#endif

#endif //TICHU_LOGGING_H
