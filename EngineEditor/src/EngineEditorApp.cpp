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
		EngineEditorApp()
		{
			LOG_INFO("EngineEditorApp created!");

			PushLayer(new EngineEditor());
		}
		~EngineEditorApp()
		{
		}
	};
}

Engine::Core::Application *Engine::Core::CreateApplication()
{
	return new EngineEditor::EngineEditorApp();
}