#include "Forest2D.h"
#include "imgui.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
Forest2D::Forest2D(const std::string &name)
    : Layer(name), m_CameraController(Engine::OrthographicCameraController(16.0f / 9.0f, 1.0f, true))
{
}

Forest2D::~Forest2D()
{
}

void Forest2D::OnAttach()
{
    LOG_INFO("Forest2D Layer attached!");
}

void Forest2D::OnDetach()
{
    LOG_INFO("Forest2D Layer detached!");
}

void Forest2D::OnUpdate(Engine::Core::Timestep timestep)
{
    m_CameraController.OnUpdate(timestep);

    Engine::Renderer::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
    Engine::Renderer::RenderCommand::Clear();

    Engine::Renderer::Renderer2D::BeginScene(m_CameraController.GetCamera());
    Engine::Renderer::Renderer2D::DrawQuad({-1.0f, 0.0f}, {0.8f, 0.8f}, m_SquareColor);

    Engine::Renderer::Renderer2D::DrawQuad({0.5f, -0.5f}, {0.4f, 0.4f}, {0.8f, 0.2f, 0.3f, 1.0f});
    Engine::Renderer::Renderer2D::EndScene();
}
void Forest2D::OnImGuiRender()
{
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
    ImGui::End();
}

bool Forest2D::OnEvent(Engine::Event::Event &event)
{
    m_CameraController.OnEvent(event);
    return false;
}
