#pragma once
#include "Core.h"
#include "Window.h"
#include "LayerStack.h"
#include "Engine/ImGui/ImGuiLayer.h"
#include "Engine/pcheader.h"
#include "Engine/Events/WindowEvent.h"
#include "Engine/Profile/ProfileLayer.h"
namespace Engine::Core
{
	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char **Args = nullptr;

		const char *operator[](int index) const
		{
			ENGINE_ASSERT(index < Count, "Index out of bounds!");
			return Args[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string Name = "Engine Application";
		uint32_t Width = 1280;
		uint32_t Height = 720;
		bool Fullscreen = false;
		bool VSync = true;
		std::string WorkingDirectory = "";
		std::string MonoAssemblyPath = "";
		ApplicationCommandLineArgs CommandLineArgs;
	};
	class Application
	{
	public:
		Application(const ApplicationSpecification &spec = ApplicationSpecification());
		virtual ~Application();

		void Run();
		void Shutdown();

		void PushLayer(Layer *layer);
		void PushOverlay(Layer *overlay);

		static Application &Get() { return *s_Instance; }

		Window &GetWindow() { return *m_Window; }
		int GetWindowWidth() const { return m_Window->GetWidth(); }
		int GetWindowHeight() const { return m_Window->GetHeight(); }
		void *GetNativeWindow() const { return m_Window->GetNativeWindow(); }

		void SubmitToMainThread(const std::function<void()> func);

		Engine::MyImGui::ImGuiLayer &GetImGuiLayer()
		{
			return *m_ImGuiLayer;
		}

		const ApplicationSpecification &GetSpecification() const { return m_Specification; }

	private:
		bool OnEvent(Event::Event &e);
		bool OnWindowClose(Event::WindowCloseEvent &e);
		bool OnWindowResize(Event::WindowResizeEvent &e);
		void ExecuteMainThreadQueue();

	private:
		bool m_Running = true;
		bool m_Minimized = false;
		ApplicationSpecification m_Specification;

		float m_LastFrameTime = 0.0f;
		Scope<Window> m_Window;
		Engine::MyImGui::ImGuiLayer *m_ImGuiLayer;

		LayerStack m_LayerStack;
		static Application *s_Instance;

		std::vector<std::function<void()>> m_MainThreadQueue;
		std::mutex m_MainThreadQueueMutex;

#if defined(FOREST_ENABLE_PROFILING)
		Engine::Profile::ProfileLayer *m_ProfileLayer;
#endif
	};
	Application *CreateApplication(ApplicationCommandLineArgs args);
}
