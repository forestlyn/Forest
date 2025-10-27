#pragma once
#ifdef FOREST_PLATFORM_WINDOWS
extern Engine::Application* Engine::CreateApplication();

int main(int args,char **argv) {
	Engine::Application* app = Engine::CreateApplication();
	app->Run();
	delete app;
	return 0;
}
#endif