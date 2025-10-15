#pragma once

// Cross-platform macro for unused parameter warning suppression

#if defined(__GNUC__) || defined(__clang__)
    // GCC and Clang support __attribute__
    #define MAYBE_UNUSED __attribute__((unused))
#elif defined(_MSC_VER)
    // MSVC doesn't support __attribute__, but has its own way
    #define MAYBE_UNUSED [[maybe_unused]]
#else
    // Fallback for other compilers
    #define MAYBE_UNUSED
#endif

// For MinGW on Windows, prefer C++17 [[maybe_unused]] if available
#if defined(__MINGW32__) || defined(__MINGW64__)
    #if __cplusplus >= 201703L
        #undef MAYBE_UNUSED
        #define MAYBE_UNUSED [[maybe_unused]]
    #endif
#endif
