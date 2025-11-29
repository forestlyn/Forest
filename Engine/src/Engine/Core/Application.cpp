#include "Application.h"
#include "Engine/pcheader.h"
#include "Engine/Events/Event.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/KeyCode.h"
#include "Engine/Renderer/RenderCommand.h"
#include "Engine/Renderer/Renderer.h"

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

		Renderer::BufferLayout layout = {
			{Renderer::ShaderDataType::Float3, "a_Position"},
			{Renderer::ShaderDataType::Float4, "a_Color"},
		};

		m_VertexArray.reset(Renderer::VertexArray::Create());
		std::shared_ptr<Renderer::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Renderer::VertexBuffer::Create(vertices, sizeof(vertices)));
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[6] = {0, 1, 2, 0, 3, 1};
		std::shared_ptr<Renderer::IndexBuffer> indexBuffer =
			std::shared_ptr<Renderer::IndexBuffer>(
				Renderer::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

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

		float vertices2[4 * 3] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.5f, 0.5f, 0.0f,
			-0.5f, 0.5f, 0.0f};
		Renderer::BufferLayout layout2 = {
			{Renderer::ShaderDataType::Float3, "a_Position"},
		};

		m_VertexArray2.reset(Renderer::VertexArray::Create());
		std::shared_ptr<Renderer::VertexBuffer> vertexBuffer2;
		vertexBuffer2.reset(Renderer::VertexBuffer::Create(vertices2, sizeof(vertices2)));
		vertexBuffer2->SetLayout(layout2);
		m_VertexArray2->AddVertexBuffer(vertexBuffer2);

		std::shared_ptr<Renderer::IndexBuffer> indexBuffer2 =
			std::shared_ptr<Renderer::IndexBuffer>(
				Renderer::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray2->SetIndexBuffer(indexBuffer2);

		std::string vertexSrc2 = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			out vec3 v_Position;
			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);
			}
		)";
		std::string fragmentSrc2 = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
			}
		)";

		m_Shader2 = std::unique_ptr<Renderer::Shader>(
			Renderer::Shader::Create(vertexSrc2, fragmentSrc2));
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_Running)
		{
			Renderer::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
			Renderer::RenderCommand::Clear();

			Renderer::Renderer::BeginScene();

			m_Shader2->Bind();
			Renderer::Renderer::Submit(m_VertexArray2);
			m_Shader->Bind();
			Renderer::Renderer::Submit(m_VertexArray);

			Renderer::Renderer::EndScene();

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