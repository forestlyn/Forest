#pragma once

#ifdef FOREST_PLATFORM_WINDOWS
#ifdef FOREST_BUILD_DLL
#define FOREST_API __declspec(dllexport)
#else
#define FOREST_API __declspec(dllimport)
#endif
#else
#error Forest only supports Windows!
#endif

#define BIT(x) (1 << x)

#ifdef FOREST_ENABLE_ASSERTS
#define ENGINE_ASSERT(x, ...)                                   \
    {                                                           \
        if (!(x))                                               \
        {                                                       \
            ENGINE_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
            __debugbreak();                                     \
        }                                                       \
    }
#define ASSERT(x, ...)                                       \
    {                                                        \
        if (!(x))                                            \
        {                                                    \
            LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
            __debugbreak();                                  \
        }                                                    \
    }
#else
#define ENGINE_ASSERT(x, ...)
#define ASSERT(x, ...)
#endif