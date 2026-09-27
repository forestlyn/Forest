#include "Log.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
namespace Engine::Core
{

	Ref<spdlog::logger> Log::s_CoreLogger;
	Ref<spdlog::logger> Log::s_ClientLogger;

    void Log::EnableFileLogging(const std::filesystem::path &path)
    {
        auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path.string(), false);
        s_CoreLogger->sinks().push_back(sink);
        s_ClientLogger->sinks().push_back(sink);
        s_CoreLogger->flush_on(spdlog::level::info);
        s_ClientLogger->flush_on(spdlog::level::info);
    }

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_CoreLogger = spdlog::stdout_color_mt("ENGINE");
		s_CoreLogger->set_level(ENGINE_LOG_LEVEL);
		s_ClientLogger = spdlog::stdout_color_mt("APP");
		s_ClientLogger->set_level(LOG_LEVEL);
	}
}