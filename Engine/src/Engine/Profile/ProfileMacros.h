// ProfileMacros.h - 改造后的宏定义
#pragma once

// 1. 全局默认开关（由CMake传入，默认关闭）
#ifndef FOREST_ENABLE_PROFILING_GLOBAL
#define FOREST_ENABLE_PROFILING_GLOBAL 0
#endif

// 2. 文件级开关（由CMake为每个文件生成，默认继承全局）
#ifndef FOREST_ENABLE_PROFILING_FILE
#define FOREST_ENABLE_PROFILING_FILE FOREST_ENABLE_PROFILING_GLOBAL
#endif

// 3. 最终生效的开关（文件级优先于全局）
#define FOREST_ENABLE_PROFILING FOREST_ENABLE_PROFILING_FILE

// 4. 原有宏逻辑（复用，仅替换开关来源）
#if FOREST_ENABLE_PROFILING == 1
#define Timer_Profiling(name)                                                               \
    Engine::Profile::Timer timer##__LINE__(name, [&](Engine::Profile::ProfileResult result) \
                                           { Engine::Profile::ProfileLayer::ProfileResults.push_back(result); })
#define ENGINE_PROFILING_SCOPE(name) \
    Engine::Profile::InstrumentorTimer instrumentorTimer##__LINE__(name)
#define ENGINE_PROFILING_FUNC() ENGINE_PROFILING_SCOPE(__FUNCSIG__)
#define ENGINE_PROFILING_BEGIN(name, filepath) \
    Engine::Profile::Instrumentor::Get().BeginSession(name, filepath)
#define ENGINE_PROFILING_END() \
    Engine::Profile::Instrumentor::Get().EndSession()
#else
#define Timer_Profiling(name)
#define ENGINE_PROFILING_FUNC()
#define ENGINE_PROFILING_SCOPE(name)
#define ENGINE_PROFILING_BEGIN(name, filepath)
#define ENGINE_PROFILING_END()
#endif