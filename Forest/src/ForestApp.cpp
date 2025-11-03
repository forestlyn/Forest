#include "Engine.h"
#include "Imgui.h"
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

	void OnImGuiRender() override
	{
		ImGui::Begin("Example Layer");
		ImGui::Text("Hello from Example Layer!");
		ImGui::End();
	}
};

class ForestApp : public Engine::Core::Application
{
public:
	ForestApp()
	{
		LOG_INFO("ForestApp created!");

		PushLayer(new ExampleLayer());
	}
	~ForestApp()
	{
	}
};

Engine::Core::Application *Engine::Core::CreateApplication()
{
	return new ForestApp();
}
