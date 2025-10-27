#include "Engine.h"

class ForestApp: public Engine::Application
{
public:
	ForestApp()
	{
	}
	~ForestApp()
	{
	}

};

Engine::Application* Engine::CreateApplication() {
	return new ForestApp();
}
