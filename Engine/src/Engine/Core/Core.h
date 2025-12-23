#pragma once
#include <memory>
#ifdef FOREST_PLATFORM_WINDOWS
#ifdef FOREST_DYNAMIC_LINK
#ifdef FOREST_BUILD_DLL
#define FOREST_API __declspec(dllexport)
#else
#define FOREST_API __declspec(dllimport)
#endif
#else
#define FOREST_API
#endif
#else
#error Forest only supports Windows!
#endif

#define BIT(x) (1 << x)
#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

#ifdef FOREST_ENABLE_ASSERTS
#define ENGINE_ASSERT_1(x)                    \
    {                                         \
        if (!(x))                             \
        {                                     \
            ENGINE_ERROR("Assertion Failed"); \
            __debugbreak();                   \
        }                                     \
    }
#define ENGINE_ASSERT_VA(x, ...)                                \
    {                                                           \
        if (!(x))                                               \
        {                                                       \
            ENGINE_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
            __debugbreak();                                     \
        }                                                       \
    }
#define ASSERT_1(x)                        \
    {                                      \
        if (!(x))                          \
        {                                  \
            LOG_ERROR("Assertion Failed"); \
            __debugbreak();                \
        }                                  \
    }
#define ASSERT_VA(x, ...)                                    \
    {                                                        \
        if (!(x))                                            \
        {                                                    \
            LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
            __debugbreak();                                  \
        }                                                    \
    }
#define GET_ASSERT_MACRO(_1, _2, NAME, ...) NAME
#define EXPAND(x) x
#define ENGINE_ASSERT(...) EXPAND(GET_ASSERT_MACRO(__VA_ARGS__, ENGINE_ASSERT_VA, ENGINE_ASSERT_1)(__VA_ARGS__))
#define ASSERT(...) EXPAND(GET_ASSERT_MACRO(__VA_ARGS__, ASSERT_VA, ASSERT_1)(__VA_ARGS__))
#else
#define ENGINE_ASSERT(x, ...)
#define ASSERT(x, ...)
#endif

namespace Engine
{
    template <typename T>
    using Scope = std::unique_ptr<T>;

    template <typename T, typename... Args>
    constexpr Scope<T> CreateScope(Args &&...args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    using Ref = std::shared_ptr<T>;

    template <typename T, typename... Args>
    constexpr Ref<T> CreateRef(Args &&...args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

}