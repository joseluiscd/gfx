#pragma once

#include <cstdlib>
#include <cstdio>

/*
Log levels:
0 -> Log nothing
1 -> Error
2 -> Warn
3 -> Info
4 -> Debug
*/
namespace gfx {

#ifndef GFX_LOGLEVEL
constexpr const unsigned log_level = 999;
#else 
constexpr const unsigned log_level = GFX_LOGLEVEL;
#endif

}

#define GFX_DEBUG(...) if constexpr (::gfx::log_level >= 4) { fprintf(stderr, "DEBUG: " __VA_ARGS__); fputs("\n", stderr); };
#define GFX_INFO(...) if constexpr (::gfx::log_level >= 3) { fprintf(stderr, "INFO: " __VA_ARGS__); fputs("\n", stderr); };
#define GFX_WARN(...) if constexpr (::gfx::log_level >= 2) { fprintf(stderr, "WARNING: " __VA_ARGS__); fputs("\n", stderr); };
#define GFX_ERROR(...) if constexpr (::gfx::log_level >= 1) { fprintf(stderr, "ERROR: " __VA_ARGS__); fputs("\n", stderr); };


#ifdef GFX_VALIDATION
#define GFX_ASSERT(CONDITION, ...) if (!static_cast<bool>(CONDITION)) { fprintf(stderr, "%s:%d: Assertion failed: %s\n", __FILE__, __LINE__, #CONDITION); fprintf(stderr, __VA_ARGS__); fputs("\n", stderr); abort(); }
#define GFX_CHECK(CONDITION, ...) if (!static_cast<bool>(CONDITION)) { fprintf(stderr, "%s:%d: Check failed: %s\n", __FILE__, __LINE__, #CONDITION); fprintf(stderr, __VA_ARGS__); fputs("\n", stderr); }
#else
#define GFX_ASSERT(...)
#define GFX_CHECK(...)
#endif