#include "Application.h"
#include "Engine/pcheader.h"
#include "Engine/Events/Event.h"
#include <glfw/glfw3.h>
namespace Engine::Core
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_Running)
		{
			glClearColor(1.0f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
		}
	}

	bool Application::OnEvent(Event::Event &e)
	{
		Event::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Event::WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		ENGINE_TRACE("{0}", e.ToString());
		return false;
	}

	bool Application::OnWindowClose(Event::WindowCloseEvent &e)
	{
		m_Running = false;
		return true;
	}
}