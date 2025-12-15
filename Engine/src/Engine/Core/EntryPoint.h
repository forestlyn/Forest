#pragma once
#ifdef FOREST_PLATFORM_WINDOWS
extern Engine::Core::Application *Engine::Core::CreateApplication();

int main(int argc, char **argv)
{
	Engine::Core::Log::Init();
	Engine::Core::Application *app;
	{
		ENGINE_PROFILING_BEGIN("Application::", "start.json");
		app = Engine::Core::CreateApplication();
		ENGINE_PROFILING_END();
	}
	{
		ENGINE_PROFILING_BEGIN("Application::Run", "run.json");
		app->Run();
		ENGINE_PROFILING_END();
	}
	{
		ENGINE_PROFILING_BEGIN("Application::Shutdown", "shutdown.json");
		delete app;
		ENGINE_PROFILING_END();
	}
	return 0;
}
#endif