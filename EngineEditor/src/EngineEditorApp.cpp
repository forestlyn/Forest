#include "Engine.h"
#include "Imgui.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "EngineEditor.h"

#include "Engine/Core/EntryPoint.h"
namespace EngineEditor
{
	class EngineEditorApp : public Engine::Core::Application
	{
	public:
		EngineEditorApp(Engine::Core::ApplicationSpecification spec) : Engine::Core::Application(spec)
		{
			LOG_INFO("EngineEditorApp created!");

			PushLayer(new EngineEditor());
		}
		~EngineEditorApp()
		{
		}
	};
}

Engine::Core::Application *Engine::Core::CreateApplication(Engine::Core::ApplicationCommandLineArgs args)
{
	Engine::Core::ApplicationSpecification spec;
	spec.Name = "Engine Editor";
	spec.Width = 1920;
	spec.Height = 1080;
	spec.Fullscreen = false;
	spec.VSync = true;
	spec.WorkingDirectory = "../../../EngineEditor/resources/";
	spec.MonoAssemblyPath = "../../Engine/ThirdParty/mono/4.5";
	spec.CommandLineArgs = args;
	return new EngineEditor::EngineEditorApp(spec);
}