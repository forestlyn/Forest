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
		std::string AppScriptPath = "";
		ApplicationCommandLineArgs CommandLineArgs;
	};
	class Application
	{
	public:
		Application(const ApplicationSpecification &spec = ApplicationSpecification());
		virtual ~Application();

		virtual void Init();
		void Run();
		void Shutdown();

		void PushLayer(Layer *layer);
		void PushOverlay(Layer *overlay);

		static Application &Get() { return *s_Instance; }

		Window &GetWindow() { return *m_Window; }
		int GetWindowWidth() const { return m_Window->GetWidth(); }
		int GetWindowHeight() const { return m_Window->GetHeight(); }
		void *GetNativeWindow() const { return m_Window->GetNativeWindow(); }

		///// @brief Submits a function to be executed on the main thread.
		//// If isFront is false, the function will be executed immediately.
		//// If isFront is true, the function will be executed when the app is not in minimized or backend state.
		///// @param func The function to execute on the main thread.
		void SubmitToMainThread(const std::function<void()> func, bool isFront = false);

		Engine::MyImGui::ImGuiLayer &GetImGuiLayer()
		{
			return *m_ImGuiLayer;
		}

		const ApplicationSpecification &GetSpecification() const { return m_Specification; }

	private:
		bool OnEvent(Event::Event &e);
		bool OnWindowClose(Event::WindowCloseEvent &e);
		bool OnWindowResize(Event::WindowResizeEvent &e);
		bool OnFocusWindow(Event::WindowFocusEvent &e);
		bool OnLostFocusWindow(Event::WindowLostFocusEvent &e);
		void ExecuteMainThreadQueueFront();
		void ExecuteMainThreadQueueBack();

	private:
		bool m_Running = true;
		bool m_Minimized = false;
		bool m_Focused = true;
		ApplicationSpecification m_Specification;

		float m_LastFrameTime = 0.0f;
		Scope<Window> m_Window;
		Engine::MyImGui::ImGuiLayer *m_ImGuiLayer;

		LayerStack m_LayerStack;
		static Application *s_Instance;

		std::vector<std::function<void()>> m_MainThreadQueueFront;
		std::vector<std::function<void()>> m_MainThreadQueueBack;
		std::mutex m_MainThreadQueueMutex;

#if defined(FOREST_ENABLE_PROFILING)
		Engine::Profile::ProfileLayer *m_ProfileLayer;
#endif
	};
	Application *CreateApplication(ApplicationCommandLineArgs args);
}
