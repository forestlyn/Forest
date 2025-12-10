#include "Engine.h"
#include "Imgui.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Forest2D.h"

#include "Engine/Core/EntryPoint.h"

class ExampleLayer : public Engine::Core::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_CameraController(45.0f, 16.0f / 9.0f, glm::bvec3(true, true, true)),
		  m_QuadPosition(0.0f, 0.0f, 0.0f), m_QuadScale(.1f, .1f, 1.f)
	{
		float vertices[4 * 5] = {
			-0.5f, -0.5f, -2.0f, 0.0f, 0.0f,
			-0.5f, 0.5f, -2.0f, 0.0f, 1.0f,
			0.5f, 0.5f, -2.0f, 1.0f, 1.0f,
			0.5f, -0.5f, -2.0f, 1.0f, 0.0f};

		Engine::Renderer::BufferLayout layout = {
			{Engine::Renderer::ShaderDataType::Float3, "a_Position"},
			{Engine::Renderer::ShaderDataType::Float2, "a_TexCoord"},
		};

		m_VertexArray = Engine::Renderer::VertexArray::Create();
		Engine::Ref<Engine::Renderer::VertexBuffer> vertexBuffer;
		vertexBuffer = Engine::Renderer::VertexBuffer::Create(vertices, sizeof(vertices));
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[6] = {0, 1, 2, 2, 3, 0};
		Engine::Ref<Engine::Renderer::IndexBuffer> indexBuffer =
			Engine::Ref<Engine::Renderer::IndexBuffer>(
				Engine::Renderer::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
			out vec3 v_Position;
			out vec2 v_TexCoord;
			void main()
			{
				v_Position = a_Position;
				v_TexCoord = a_TexCoord;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			uniform sampler2D u_Texture;
			in vec3 v_Position;
			in vec2 v_TexCoord;
			void main()
			{
				color = texture(u_Texture, v_TexCoord);
				// color = vec4(0.2, 0.3, 0.8, 1.0); 
			}
		)";

		auto m_Shader = Engine::Ref<Engine::Renderer::Shader>(
			Engine::Renderer::Shader::Create("TextureShader", vertexSrc, fragmentSrc));
		m_ShaderLibrary.Add(m_Shader);

		float vertices2[4 * 3] = {
			-0.5f, -0.5f, -2.1f,
			0.5f, -0.5f, -2.1f,
			0.5f, 0.5f, -2.1f,
			-0.5f, 0.5f, -2.1f};
		Engine::Renderer::BufferLayout layout2 = {
			{Engine::Renderer::ShaderDataType::Float3, "a_Position"},
		};

		m_VertexArray2 = Engine::Renderer::VertexArray::Create();
		Engine::Ref<Engine::Renderer::VertexBuffer> vertexBuffer2;
		vertexBuffer2 = Engine::Renderer::VertexBuffer::Create(vertices2, sizeof(vertices2));
		vertexBuffer2->SetLayout(layout2);
		m_VertexArray2->AddVertexBuffer(vertexBuffer2);

		uint32_t indices2[6] = {0, 1, 2, 2, 3, 0};
		Engine::Ref<Engine::Renderer::IndexBuffer> indexBuffer2 =
			Engine::Ref<Engine::Renderer::IndexBuffer>(
				Engine::Renderer::IndexBuffer::Create(indices2, sizeof(indices2) / sizeof(uint32_t)));
		m_VertexArray2->SetIndexBuffer(indexBuffer2);

		m_ShaderLibrary.Load("assets/shaders/ColorShader.glsl");

		m_CheckerBoardTexture = Engine::Ref<Engine::Renderer::Texture2D>(
			Engine::Renderer::Texture2D::Create("assets/textures/Checkerboard.png"));

		m_LogoTexture = Engine::Ref<Engine::Renderer::Texture2D>(
			Engine::Renderer::Texture2D::Create("assets/textures/ForestLogo.png"));

		m_Shader->Bind();
		dynamic_cast<Platform::OpenGL::OpenGLShader &>(*m_Shader).UploadUniformInt("u_Texture", 0);
	}

	void OnAttach() override
	{
	}

	void OnDetach() override
	{
	}

	void OnUpdate(Engine::Core::Timestep timestep) override
	{
		m_CameraController.OnUpdate(timestep);

		if (Engine::Core::Input::IsKeyPressed(FOREST_KEY_I))
			m_QuadPosition.y += m_QuadMoveSpeed * timestep;
		if (Engine::Core::Input::IsKeyPressed(FOREST_KEY_K))
			m_QuadPosition.y -= m_QuadMoveSpeed * timestep;
		if (Engine::Core::Input::IsKeyPressed(FOREST_KEY_J))
			m_QuadPosition.x -= m_QuadMoveSpeed * timestep;
		if (Engine::Core::Input::IsKeyPressed(FOREST_KEY_L))
			m_QuadPosition.x += m_QuadMoveSpeed * timestep;

		// float ts = timestep;
		// LOG_TRACE("{0}", ts);

		Engine::Renderer::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
		Engine::Renderer::RenderCommand::Clear();

		Engine::Renderer::Renderer::BeginScene(m_CameraController.GetCamera());

		auto m_Shader2 = m_ShaderLibrary.Get("ColorShader");
		m_Shader2->Bind();
		dynamic_cast<Platform::OpenGL::OpenGLShader &>(*m_Shader2).UploadUniformFloat3("u_Color", m_SquareColor);
		float posdelta = 0.11f;
		for (int x = 0; x < 20; x++)
		{
			for (int y = 0; y < 20; y++)
			{
				Engine::Renderer::Renderer::Submit(m_Shader2, m_VertexArray2,
												   glm::translate(glm::mat4(1.0f), m_QuadPosition + glm::vec3(x * posdelta, y * posdelta, 0.0f)) *
													   glm::scale(glm::mat4(1.0f), m_QuadScale));
			}
		}

		auto m_Shader = m_ShaderLibrary.Get("TextureShader");
		m_CheckerBoardTexture->Bind(0);
		Engine::Renderer::Renderer::Submit(m_Shader, m_VertexArray,
										   glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) *
											   glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f)));

		m_LogoTexture->Bind(0);
		Engine::Renderer::Renderer::Submit(m_Shader, m_VertexArray,
										   glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.1f)) *
											   glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f)));

		Engine::Renderer::Renderer::EndScene();
	}

	bool OnEvent(Engine::Event::Event &event) override
	{
		m_CameraController.OnEvent(event);
		return false;
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

private:
	Engine::Ref<Engine::Renderer::VertexArray> m_VertexArray;

	Engine::Ref<Engine::Renderer::VertexArray> m_VertexArray2;

	Engine::Renderer::ShaderLibrary m_ShaderLibrary;

	Engine::Ref<Engine::Renderer::Texture2D> m_CheckerBoardTexture, m_LogoTexture;

	Engine::PerspectiveCameraController m_CameraController;

	glm::vec3 m_QuadPosition;
	glm::vec3 m_QuadScale;
	float m_QuadMoveSpeed = 1.0f;

	glm::vec3 m_SquareColor = {0.2f, 0.3f, 0.8f};
};

class ForestApp : public Engine::Core::Application
{
public:
	ForestApp()
	{
		LOG_INFO("ForestApp created!");

		PushLayer(new ExampleLayer());
		// PushLayer(new Forest2D());
	}
	~ForestApp()
	{
	}
};

Engine::Core::Application *Engine::Core::CreateApplication()
{
	return new ForestApp();
}
