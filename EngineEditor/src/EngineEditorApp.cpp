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
		EngineEditorApp(Engine::Core::ApplicationCommandLineArgs args)
			: Engine::Core::Application(Engine::Core::ApplicationSpecification{"Engine Editor", 1600, 900, true, true, args})
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
	return new EngineEditor::EngineEditorApp(args);
}