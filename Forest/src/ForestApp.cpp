#include "Engine.h"
#include "Imgui.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Forest2D.h"

#include "Engine/Core/EntryPoint.h"

class ForestApp : public Engine::Core::Application
{
public:
	ForestApp(Engine::Core::ApplicationSpecification spec) : Engine::Core::Application(spec)
	{
		LOG_INFO("ForestApp created!");

		// PushLayer(new ExampleLayer());
		PushLayer(new Forest2D());
	}
	~ForestApp()
	{
	}
};

Engine::Core::Application *Engine::Core::CreateApplication(Engine::Core::ApplicationCommandLineArgs args)
{
	Engine::Core::ApplicationSpecification spec;
	spec.Name = "Forest";
	spec.Width = 1600;
	spec.Height = 900;
	spec.Fullscreen = false;
	spec.VSync = true;
	spec.CommandLineArgs = args;
	spec.WorkingDirectory = "../../Forest";
	spec.MonoAssemblyPath = "../Engine/ThirdParty/mono/4.5";
	return new ForestApp(spec);
}