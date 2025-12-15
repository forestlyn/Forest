#include "Application.h"
#include "Engine/pcheader.h"
#include "Engine/Events/Event.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/KeyCode.h"
#include "GLFW/glfw3.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Profile/Instrumentor.h"
namespace Engine::Core
{

	Application *Application::s_Instance = nullptr;

	Application::Application()
	{

		ENGINE_PROFILING_SCOPE("START");
		m_Window = Scope<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		Renderer::Renderer::Init();

		ENGINE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		// imgui init needs to be after window creation and s_Instance assignment
		m_ImGuiLayer = new MyImGui::ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

#if defined(FOREST_ENABLE_PROFILING)
		m_ProfileLayer = new Engine::Profile::ProfileLayer();
		PushOverlay(m_ProfileLayer);
#endif
	}

	Application::~Application()
	{
		ENGINE_PROFILING_FUNC();
		Renderer::Renderer::Shutdown();
	}

	void Application::Run()
	{
		ENGINE_PROFILING_FUNC();
		while (m_Running)
		{
			{
				ENGINE_PROFILING_SCOPE("Run Loop");
				Timestep time = glfwGetTime(); // Get time in seconds
				float deltaTime = time - m_LastFrameTime;
				m_LastFrameTime = time;
				if (!m_Minimized)
				{
					ENGINE_PROFILING_SCOPE("LayerStack OnUpdate");
					for (auto layer : m_LayerStack)
						layer->OnUpdate(deltaTime);
				}
				{
					ENGINE_PROFILING_SCOPE("LayerStack OnImGuiRender");
					{
						ENGINE_PROFILING_SCOPE("LayerStack OnImGuiRender Begin");
						m_ImGuiLayer->Begin();
					}
					{
						ENGINE_PROFILING_SCOPE("Layers OnImGuiRender");
						for (auto layer : m_LayerStack)
						{
							layer->OnImGuiRender();
						}
					}

					{
						ENGINE_PROFILING_SCOPE("LayerStack OnImGuiRender End");
						m_ImGuiLayer->End();
					}
				}

				{
					ENGINE_PROFILING_SCOPE("Window OnUpdate");
					m_Window->OnUpdate();
				}
			}
		}
	}

	bool Application::OnEvent(Event::Event &e)
	{
		ENGINE_PROFILING_FUNC();
		Event::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Event::WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<Event::WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}

		return false;
	}

	bool Application::OnWindowClose(Event::WindowCloseEvent &e)
	{
		ENGINE_PROFILING_FUNC();

		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(Event::WindowResizeEvent &e)
	{
		ENGINE_PROFILING_FUNC();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
		}
		else
		{
			m_Minimized = false;
			Renderer::Renderer::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
		}
		return false;
	}

	void Application::PushLayer(Layer *layer)
	{
		ENGINE_PROFILING_FUNC();
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer *overlay)
	{
		ENGINE_PROFILING_FUNC();
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}
}