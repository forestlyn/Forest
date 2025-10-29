#include "Engine.h"

class ForestApp : public Engine::Core::Application
{
public:
	ForestApp()
	{
	}
	~ForestApp()
	{
	}
};

Engine::Core::Application *Engine::Core::CreateApplication()
{
	return new ForestApp();
}
