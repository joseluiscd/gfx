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

#define GFX_DEBUG(MSG, ...) if constexpr (log_level >= 4) { fprintf(stderr, "DEBUG: " MSG "\n", ##__VA_ARGS__); };
#define GFX_INFO(MSG, ...) if constexpr (log_level >= 3) { fprintf(stderr, "INFO: " MSG "\n", ##__VA_ARGS__); };
#define GFX_WARN(MSG, ...) if constexpr (log_level >= 2) { fprintf(stderr, "WARNING: " MSG "\n", ##__VA_ARGS__); };
#define GFX_ERROR(MSG, ...) if constexpr (log_level >= 1) { fprintf(stderr, "ERROR: " MSG "\n", ##__VA_ARGS__); };

#ifdef GFX_VALIDATION
#define GFX_ASSERT(CONDITION, MSG, ...) if (!static_cast<bool>(CONDITION)) { fprintf(stderr, "%s:%d: Assertion failed: %s\n", __FILE__, __LINE__, #CONDITION); fprintf(stderr, MSG "\n", ##__VA_ARGS__); abort(); }
#define GFX_CHECK(CONDITION, MSG, ...) if (!static_cast<bool>(CONDITION)) { fprintf(stderr, "%s:%d: Assertion failed: %s\n", __FILE__, __LINE__, #CONDITION); fprintf(stderr, MSG "\n", ##__VA_ARGS__); }
#else
#define GFX_ASSERT(...)
#define GFX_CHECK(...)
#endif