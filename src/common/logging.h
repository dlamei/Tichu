#ifndef TICHU_LOGGING_H
#define TICHU_LOGGING_H

#include <spdlog/spdlog.h>

inline void init_logger() {
    spdlog::set_level(spdlog::level::trace);
}

#define DEBUG(...) ::spdlog::debug(__VA_ARGS__)
#define INFO(...) ::spdlog::info(__VA_ARGS__)
#define WARN(...) ::spdlog::warn(__VA_ARGS__)
#ifdef ERROR
#undef ERROR
#endif
#define ERROR(...) ::spdlog::error(__VA_ARGS__)

#if NDEBUG
#define ASSERT(x, ...)
#else
#define ASSERT(x, ...) { if(!(x)) {ERROR("Assertion Failed: {0}", __VA_ARGS__); assert(false); } }
#endif

#endif //TICHU_LOGGING_H
