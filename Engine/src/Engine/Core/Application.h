#pragma once
#include "Core.h"
#include "Window.h"
#include "LayerStack.h"
#include "Engine/ImGui/ImGuiLayer.h"
#include "Engine/pcheader.h"
#include "Engine/Events/WindowEvent.h"
namespace Engine::Core
{

	class FOREST_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void PushLayer(Layer *layer);
		void PushOverlay(Layer *overlay);

		static Application &Get() { return *s_Instance; }

		Window &GetWindow() { return *m_Window; }
		int GetWindowWidth() const { return m_Window->GetWidth(); }
		int GetWindowHeight() const { return m_Window->GetHeight(); }
		void *GetNativeWindow() const { return m_Window->GetNativeWindow(); }

	private:
		bool m_Running = true;
		bool m_Minimized = false;

		float m_LastFrameTime = 0.0f;
		Scope<Window> m_Window;
		Engine::MyImGui::ImGuiLayer *m_ImGuiLayer;

		bool OnEvent(Event::Event &e);
		bool OnWindowClose(Event::WindowCloseEvent &e);
		bool OnWindowResize(Event::WindowResizeEvent &e);

		LayerStack m_LayerStack;
		static Application *s_Instance;
	};
	Application *CreateApplication();
}
