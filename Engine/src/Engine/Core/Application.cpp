#include "Application.h"
#include "Engine/pcheader.h"
#include "Engine/Events/Event.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/KeyCode.h"
#include <glad/glad.h>

GLenum GetGLenumFromShaderDataType(Engine::Renderer::ShaderDataType type)
{
	switch (type)
	{
	case Engine::Renderer::ShaderDataType::Float:
	case Engine::Renderer::ShaderDataType::Float2:
	case Engine::Renderer::ShaderDataType::Float3:
	case Engine::Renderer::ShaderDataType::Float4:
	case Engine::Renderer::ShaderDataType::Mat3:
	case Engine::Renderer::ShaderDataType::Mat4:
		return GL_FLOAT;
	case Engine::Renderer::ShaderDataType::Int:
	case Engine::Renderer::ShaderDataType::Int2:
	case Engine::Renderer::ShaderDataType::Int3:
	case Engine::Renderer::ShaderDataType::Int4:
		return GL_INT;
	case Engine::Renderer::ShaderDataType::Bool:
		return GL_BOOL;
	default:
		ENGINE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}
}

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
		m_ImGuiLayer = new MyImGui::ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		float vertices[4 * 7] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};

		// float vertices[4 * 3] = {
		// 	-0.5f, -0.5f, 0.0f,
		// 	0.5f, -0.5f, 0.0f,
		// 	0.0f, 0.5f, 0.0f,
		// 	0.0f, -1.0f, 0.0f};

		Renderer::BufferLayout layout = {
			{Renderer::ShaderDataType::Float3, "a_Position"},
			{Renderer::ShaderDataType::Float4, "a_Color"},
		};

		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		m_VertexBuffer = std::unique_ptr<Renderer::VertexBuffer>(
			Renderer::VertexBuffer::Create(vertices, sizeof(vertices)));
		m_VertexBuffer->SetLayout(layout);
		m_VertexBuffer->Bind();

		int index = 0;
		for (const auto &element : m_VertexBuffer->GetLayout())
		{
			ENGINE_TRACE("index:{0} Buffer Element: {1}, Type: {2}, Size: {3}, Offset: {4}",
						 index, element.Name, static_cast<int>(element.Type), element.Size, element.Offset);
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
								  element.GetComponentCount(),
								  GetGLenumFromShaderDataType(element.Type),
								  element.Normalized ? GL_TRUE : GL_FALSE,
								  m_VertexBuffer->GetLayout().GetStride(),
								  reinterpret_cast<const void *>(element.Offset));
			index++;
		}
		// glEnableVertexAttribArray(0);
		// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void *)0);

		uint32_t indices[6] = {0, 1, 2, 0, 3, 1};
		m_IndexBuffer = std::unique_ptr<Renderer::IndexBuffer>(
			Renderer::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));

		m_IndexBuffer->Bind();

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;
			out vec3 v_Position;
			out vec4 v_Color;
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			in vec4 v_Color;
			void main()
			{
				color = v_Color;
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
			glBindVertexArray(m_VertexArray);
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