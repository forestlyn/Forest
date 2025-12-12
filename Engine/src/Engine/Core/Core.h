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

#if defined(FOREST_ENABLE_PROFILING)
#define Timer_Profiling(name)                                                               \
    Engine::Profile::Timer timer##__LINE__(name, [&](Engine::Profile::ProfileResult result) \
                                           { Engine::Profile::ProfileLayer::ProfileResults.push_back(result); });
#else
#define Timer_Profiling(name)
#endif

namespace Engine
{
    template <typename T>
    using Scope = std::unique_ptr<T>;

    template <typename T>
    using Ref = std::shared_ptr<T>;
}