#ifndef TICHU_LOGGING_H
#define TICHU_LOGGING_H

#include <spdlog/spdlog.h>

#define TRACE(...) ::spdlog::trace(__VA_ARGS__)
#define INFO(...) ::spdlog::info(__VA_ARGS__)
#define WARN(...) ::spdlog::warn(__VA_ARGS__)
#define ERROR(...) ::spdlog::error(__VA_ARGS__)

#if NDEBUG
#define ASSERT(x, ...)
#else
#define ASSERT(x, ...) { if(!x) {ERROR("Assertion Failed: {0}", __VA_ARGS__); exit(-1); } }
#endif

#endif //TICHU_LOGGING_H
