#pragma once
#ifdef FOREST_PLATFORM_WINDOWS
extern Engine::Core::Application *Engine::Core::CreateApplication();

int main(int argc, char **argv)
{
	Engine::Core::Log::Init();
	Engine::Core::Application *app = Engine::Core::CreateApplication();
	app->Run();
	delete app;
	return 0;
}
#endif