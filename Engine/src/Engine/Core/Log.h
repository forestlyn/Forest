#pragma once
#include "Core.h"
#include "spdlog/spdlog.h"
#include "Engine/pcheader.h"

namespace Engine::Core
{
	class FOREST_API Log
	{
	public:
		static void Init();
		inline static std::shared_ptr<spdlog::logger> &GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger> &GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Engine log macros
#define ENGINE_TRACE(...) ::Engine::Core::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ENGINE_INFO(...) ::Engine::Core::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ENGINE_WARN(...) ::Engine::Core::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ENGINE_ERROR(...) ::Engine::Core::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ENGINE_CRITICAL(...) ::Engine::Core::Log::GetCoreLogger()->critical(__VA_ARGS__)
// App log macros
#define LOG_TRACE(...) ::Engine::Core::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...) ::Engine::Core::Log::GetClientLogger()->info(__VA_ARGS__)
#define LOG_WARN(...) ::Engine::Core::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ::Engine::Core::Log::GetClientLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Engine::Core::Log::GetClientLogger()->critical(__VA_ARGS__)