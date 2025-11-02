#include "Engine.h"
#include "Engine/Math/Math.h"
class ExampleLayer : public Engine::Core::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnAttach() override
	{
	}

	void OnDetach() override
	{
	}

	void OnUpdate() override
	{
	}

	bool OnEvent(Engine::Event::Event &event) override
	{
		return false;
	}
};

class ForestApp : public Engine::Core::Application
{
public:
	ForestApp()
	{
		LOG_INFO("ForestApp created!");

		PushLayer(new ExampleLayer());
		PushOverlay(new Engine::MyImGui::ImGuiLayer());
	}
	~ForestApp()
	{
	}
};

Engine::Core::Application *Engine::Core::CreateApplication()
{
	return new ForestApp();
}
