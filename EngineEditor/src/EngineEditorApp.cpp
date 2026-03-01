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
#include <filewatch.hpp>
#include "Engine/Scripts/ScriptEngine.h"
#include <chrono>
namespace EngineEditor
{
	class EngineEditorApp : public Engine::Core::Application
	{
	private:
		Engine::Scope<filewatch::FileWatch<std::string>> m_AppScriptFileWatcher;
		static bool m_IsCompilingScripts;

	public:
		EngineEditorApp(Engine::Core::ApplicationSpecification spec) : Engine::Core::Application(spec)
		{
			LOG_INFO("EngineEditorApp created!");

			PushLayer(new EngineEditor());
		}
		~EngineEditorApp()
		{
		}

		void Init() override
		{
			ENGINE_INFO("Initializing EngineEditorApp...");
			Engine::Core::Application::Init();
			m_AppScriptFileWatcher = Engine::CreateScope<filewatch::FileWatch<std::string>>(
				GetSpecification().AppScriptPath, AppScriptFileWatcherChangeEvent);
			m_IsCompilingScripts = false;
		}

		static void AppScriptFileWatcherChangeEvent(const std::string &path, const filewatch::Event event)
		{
			if (m_IsCompilingScripts)
				return;
			m_IsCompilingScripts = true;
			if (event == filewatch::Event::modified)
			{
				Application::Get().SubmitToMainThread(
					[]()
					{
						CompileCSharpScripts();
						m_IsCompilingScripts = false;
					},
					true);
			}
		}
		static bool CompileCSharpScripts()
		{
			ENGINE_INFO("Compiling C# scripts...");

			std::string command = "cmake --build ./../../build --config Debug --target Sandbox 2>&1";

			// 使用 popen 执行命令并捕获输出
			FILE *pipe = _popen(command.c_str(), "r");
			if (!pipe)
			{
				ENGINE_ERROR("Failed to start C# compilation process!");
				return false;
			}

			char buffer[128];
			std::string output;

			// 实时读取并显示输出
			while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
			{
				output += buffer;
			}
			ENGINE_INFO("C# Compilation Output:\n{}", output);

			int exitCode = _pclose(pipe);

			if (exitCode == 0)
			{
				ENGINE_INFO("C# scripts compiled successfully!");
				return true;
			}
			else
			{
				ENGINE_ERROR("C# scripts compilation failed with error code: {}", exitCode);
				return false;
			}
		}
	};

	bool EngineEditorApp::m_IsCompilingScripts = false;

}

Engine::Core::Application *
Engine::Core::CreateApplication(Engine::Core::ApplicationCommandLineArgs args)
{
	Engine::Core::ApplicationSpecification spec;
	spec.Name = "Engine Editor";
	spec.Width = 1920;
	spec.Height = 1080;
	spec.Fullscreen = false;
	spec.VSync = true;
	spec.WorkingDirectory = "../../../EngineEditor/resources/";
	spec.MonoAssemblyPath = "../../Engine/ThirdParty/mono/4.5";
	spec.AppScriptPath = "../../EngineEditor/Sandbox/Assets/Scripts/src";
	spec.CommandLineArgs = args;
	return new EngineEditor::EngineEditorApp(spec);
}