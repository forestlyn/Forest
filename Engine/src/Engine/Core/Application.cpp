#include "Application.h"
#include "Engine/pcheader.h"
#include "Engine/Events/Event.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/KeyCode.h"
#include <glad/glad.h>
#include "imgui.h"
namespace Engine::Core
{

	Application *Application::s_Instance = nullptr;

	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		ENGINE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		// imgui init needs to be after window creation and s_Instance assignment
		m_ImGuiLayer = std::make_unique<Engine::MyImGui::ImGuiLayer>();
		PushOverlay(m_ImGuiLayer.get());

		float vertices[4 * 3] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f, 0.5f, 0.0f,
			0.0f, -1.0f, 0.0f};

		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		m_VertexBuffer = std::unique_ptr<Renderer::VertexBuffer>(
			Renderer::VertexBuffer::Create(vertices, 4 * 3 * sizeof(float)));
		m_VertexBuffer->Bind();

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void *)0);

		unsigned int indices[6] = {0, 1, 2, 0, 3, 1};
		m_IndexBuffer = std::unique_ptr<Renderer::IndexBuffer>(
			Renderer::IndexBuffer::Create(indices, sizeof(indices) / sizeof(unsigned int)));

		m_IndexBuffer->Bind();

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			out vec3 v_Position;
			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
			}
		)";
		m_Shader = std::unique_ptr<Renderer::Shader>(
			Renderer::Shader::Create(vertexSrc, fragmentSrc));
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_Running)
		{
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Shader->Bind();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			for (auto layer : m_LayerStack)
				layer->OnUpdate();

			m_ImGuiLayer->Begin();
			for (auto layer : m_LayerStack)
			{
				layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	bool Application::OnEvent(Event::Event &e)
	{
		Event::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Event::WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

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
		m_Running = false;
		return true;
	}

	void Application::PushLayer(Layer *layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer *overlay)
	{
		m_LayerStack.PushOverlay(overlay);
	}
}