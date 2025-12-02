#include "Engine.h"
#include "Imgui.h"
#include "Engine/Renderer/RenderCommand.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Buffer.h"
#include "Engine/Renderer/VertexArray.h"
#include "Engine/Renderer/Camera/Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

class ExampleLayer : public Engine::Core::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(new Engine::Renderer::PerspectiveCamera(45.0f, 16.0f / 9.0f, 0.1f, 100.0f)),
		  m_CameraPosition(0.0f, 0.0f, 0.0f), m_CameraRotation(0.0f, 0.0f, 0.0f),
		  m_QuadPosition(0.0f, 0.0f, 0.0f), m_QuadScale(1.0f, 1.0f, 1.0f)
	{
		float vertices[4 * 7] = {
			-0.5f, -0.5f, -5.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			0.5f, -0.5f, -5.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			0.0f, 0.5f, -5.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			0.0f, -1.0f, -5.0f, 1.0f, 1.0f, 0.0f, 1.0f};

		Engine::Renderer::BufferLayout layout = {
			{Engine::Renderer::ShaderDataType::Float3, "a_Position"},
			{Engine::Renderer::ShaderDataType::Float4, "a_Color"},
		};

		m_VertexArray.reset(Engine::Renderer::VertexArray::Create());
		std::shared_ptr<Engine::Renderer::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Engine::Renderer::VertexBuffer::Create(vertices, sizeof(vertices)));
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[6] = {0, 1, 2, 0, 3, 1};
		std::shared_ptr<Engine::Renderer::IndexBuffer> indexBuffer =
			std::shared_ptr<Engine::Renderer::IndexBuffer>(
				Engine::Renderer::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
			out vec3 v_Position;
			out vec4 v_Color;
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
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
		m_Shader = std::shared_ptr<Engine::Renderer::Shader>(
			Engine::Renderer::Shader::Create(vertexSrc, fragmentSrc));

		float vertices2[4 * 3] = {
			-0.5f, -0.0f, -1.0f,
			0.5f, -0.5f, -1.0f,
			0.5f, 0.5f, -1.0f,
			-0.5f, 0.5f, -1.0f};
		Engine::Renderer::BufferLayout layout2 = {
			{Engine::Renderer::ShaderDataType::Float3, "a_Position"},
		};

		m_VertexArray2.reset(Engine::Renderer::VertexArray::Create());
		std::shared_ptr<Engine::Renderer::VertexBuffer> vertexBuffer2;
		vertexBuffer2.reset(Engine::Renderer::VertexBuffer::Create(vertices2, sizeof(vertices2)));
		vertexBuffer2->SetLayout(layout2);
		m_VertexArray2->AddVertexBuffer(vertexBuffer2);

		std::shared_ptr<Engine::Renderer::IndexBuffer> indexBuffer2 =
			std::shared_ptr<Engine::Renderer::IndexBuffer>(
				Engine::Renderer::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray2->SetIndexBuffer(indexBuffer2);

		std::string vertexSrc2 = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			out vec3 v_Position;
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
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

		m_Shader2 = std::shared_ptr<Engine::Renderer::Shader>(
			Engine::Renderer::Shader::Create(vertexSrc2, fragmentSrc2));
	}

	void OnAttach() override
	{
	}

	void OnDetach() override
	{
	}

	void OnUpdate(Engine::Core::Timestep timestep) override
	{
		if (Engine::Core::Input::IsKeyPressed(FOREST_KEY_UP))
			m_CameraPosition.y -= m_CameraMoveSpeed * timestep;
		if (Engine::Core::Input::IsKeyPressed(FOREST_KEY_DOWN))
			m_CameraPosition.y += m_CameraMoveSpeed * timestep;
		if (Engine::Core::Input::IsKeyPressed(FOREST_KEY_LEFT))
			m_CameraPosition.x -= m_CameraMoveSpeed * timestep;
		if (Engine::Core::Input::IsKeyPressed(FOREST_KEY_RIGHT))
			m_CameraPosition.x += m_CameraMoveSpeed * timestep;

		if (Engine::Core::Input::IsKeyPressed(FOREST_KEY_A))
			m_CameraRotation.z -= m_CameraRotationSpeed * timestep;
		if (Engine::Core::Input::IsKeyPressed(FOREST_KEY_D))
			m_CameraRotation.z += m_CameraRotationSpeed * timestep;

		if (Engine::Core::Input::IsKeyPressed(FOREST_KEY_I))
			m_QuadPosition.y += m_QuadMoveSpeed * timestep;
		if (Engine::Core::Input::IsKeyPressed(FOREST_KEY_K))
			m_QuadPosition.y -= m_QuadMoveSpeed * timestep;
		if (Engine::Core::Input::IsKeyPressed(FOREST_KEY_J))
			m_QuadPosition.x -= m_QuadMoveSpeed * timestep;
		if (Engine::Core::Input::IsKeyPressed(FOREST_KEY_L))
			m_QuadPosition.x += m_QuadMoveSpeed * timestep;

		if (Engine::Core::Input::IsKeyPressed(FOREST_KEY_W))
			m_QuadScale += glm::vec3(1.0f, 1.0f, 0.0f) * (m_QuadScaleSpeed * timestep);
		if (Engine::Core::Input::IsKeyPressed(FOREST_KEY_S))
			m_QuadScale -= glm::vec3(1.0f, 1.0f, 0.0f) * (m_QuadScaleSpeed * timestep);

		Engine::Renderer::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
		Engine::Renderer::RenderCommand::Clear();
		m_Camera->SetPosition(m_CameraPosition);
		m_Camera->SetRotationDegrees(m_CameraRotation);

		Engine::Renderer::Renderer::BeginScene(*m_Camera);

		Engine::Renderer::Renderer::Submit(m_Shader2, m_VertexArray2);
		Engine::Renderer::Renderer::Submit(m_Shader, m_VertexArray,
										   glm::translate(glm::mat4(1.0f), m_QuadPosition) *
											   glm::scale(glm::mat4(1.0f), m_QuadScale));

		Engine::Renderer::Renderer::EndScene();
	}

	bool OnEvent(Engine::Event::Event &event) override
	{
		return false;
	}

	void OnImGuiRender() override
	{
	}

private:
	std::shared_ptr<Engine::Renderer::VertexArray> m_VertexArray;
	std::shared_ptr<Engine::Renderer::Shader> m_Shader;

	std::shared_ptr<Engine::Renderer::VertexArray> m_VertexArray2;
	std::shared_ptr<Engine::Renderer::Shader> m_Shader2;

	Engine::Renderer::Camera *m_Camera;

	glm::vec3 m_CameraPosition;
	glm::vec3 m_CameraRotation;
	float m_CameraMoveSpeed = 1.f;
	float m_CameraRotationSpeed = 20.0f;

	glm::vec3 m_QuadPosition;
	glm::vec3 m_QuadScale;
	float m_QuadMoveSpeed = 1.0f;
	float m_QuadScaleSpeed = 1.0f;
};

class ForestApp : public Engine::Core::Application
{
public:
	ForestApp()
	{
		LOG_INFO("ForestApp created!");

		PushLayer(new ExampleLayer());
	}
	~ForestApp()
	{
	}
};

Engine::Core::Application *Engine::Core::CreateApplication()
{
	return new ForestApp();
}
