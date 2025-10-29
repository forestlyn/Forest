#pragma once
#ifdef FOREST_PLATFORM_WINDOWS
extern Engine::Core::Application *Engine::Core::CreateApplication();

int main(int argc, char **argv)
{
	Engine::Core::Log::Init();
	ENGINE_INFO("hjaha	");

	Engine::Event::KeyPressedEvent event(32, true);
	ENGINE_INFO("{0}", event.ToString());
	if (event.IsInCategory(Engine::Event::EventCategoryKeyboard))
	{
		ENGINE_INFO("is keyboard event");
	}

	Engine::Event::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<Engine::Event::KeyPressedEvent>([](Engine::Event::KeyPressedEvent &e)
														{
			ENGINE_INFO("KeyPressedEvent dispatched");
			return true; });

	Engine::Core::Application *app = Engine::Core::CreateApplication();
	app->Run();
	delete app;
	return 0;
}
#endif