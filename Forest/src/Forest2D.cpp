#include "Forest2D.h"
#include "imgui.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Engine/Profile/ProfileLayer.h"
#include "Engine/Profile/Instrumentor.h"

Forest2D::Forest2D(const std::string &name)
    : Layer(name), m_CameraController(Engine::OrthographicCameraController(16.0f / 9.0f, 1.0f, true))
{
    m_CheckerBoardTexture = Engine::Renderer::Texture2D::Create("assets/textures/Checkerboard.png");
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
    // InstrumentorProfilingBegin("Forest2D::OnUpdate");
    {
        ENGINE_PROFILING_FUNC();
        {
            ENGINE_PROFILING_SCOPE("m_CameraController::OnUpdate");
            m_CameraController.OnUpdate(timestep);
        }
        {
            ENGINE_PROFILING_SCOPE("Forest2D::PreRenderer");
            Engine::Renderer::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
            Engine::Renderer::RenderCommand::Clear();
        }

        {
            {
                ENGINE_PROFILING_SCOPE("Renderer2D::BeginScene");
                Engine::Renderer::Renderer2D::BeginScene(m_CameraController.GetCamera());
            }
            {
                ENGINE_PROFILING_SCOPE("Renderer2D::DrawQuad");
                Engine::Renderer::Renderer2D::DrawQuad({-1.0f, 0.0f}, {0.8f, 0.8f}, m_SquareColor);
                Engine::Renderer::Renderer2D::DrawQuad({0.5f, -0.5f}, {0.4f, 0.4f}, {0.8f, 0.2f, 0.3f, 1.0f});
                Engine::Renderer::Renderer2D::DrawQuad({0.0f, 0.0f, -0.1f}, {1.0f, 1.0f}, m_CheckerBoardTexture);
                Engine::Renderer::Renderer2D::DrawQuad({-2.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, 45.0f, m_CheckerBoardTexture, {0.8f, 1.0f, 0.3f, 1.0f});
            }
            {
                ENGINE_PROFILING_SCOPE("Renderer2D::EndScene");
                Engine::Renderer::Renderer2D::EndScene();
            }
        }
    }
    // InstrumentorProfilingEnd();
}
void Forest2D::OnImGuiRender()
{
    ENGINE_PROFILING_FUNC();
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
    ImGui::End();
}

bool Forest2D::OnEvent(Engine::Event::Event &event)
{
    m_CameraController.OnEvent(event);
    return false;
}
