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
    {
        ENGINE_PROFILING_FUNC();

        m_CameraController.OnUpdate(timestep);

        {
            ENGINE_PROFILING_SCOPE("Forest2D::PreRenderer");
            Engine::Renderer::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
            Engine::Renderer::RenderCommand::Clear();
        }

        {
            Timer_Profiling("Forest2D::Renderer2D Scene");

            Engine::Renderer::Renderer2D::BeginScene(m_CameraController.GetCamera());
            Engine::Renderer::Renderer2D::ResetStats();
            {
                ENGINE_PROFILING_SCOPE("Renderer2D::DrawQuad");
                for (uint32_t i = 0; i < 10000; i++)
                {
                    float x = (i % 100) * 0.11f - 5.0f;
                    float y = (i / 100) * 0.11f - 5.0f;
                    Engine::Renderer::Renderer2D::DrawQuad({x, y}, {0.1f, 0.1f}, {i % 255 / 255.0f, 0.3f, i / 255.0f, 1.0f});
                }

                // when debugging, just draw a few quads , the 4th and 2th with texture cannt be drawn at the same time
                Engine::Renderer::Renderer2D::DrawQuad({0.5f, -0.5f, 0.1f}, {0.4f, 0.4f}, {0.8f, 0.2f, 0.3f, 1.0f});
                Engine::Renderer::Renderer2D::DrawQuad({0.0f, 0.0f, 0.1f}, {1.0f, 1.0f}, m_CheckerBoardTexture);
                Engine::Renderer::Renderer2D::DrawRotateQuad({0.5f, 0.5f, 0.2f}, {1.0f, 1.0f}, {0.8f, 1.0f, 0.3f, 1.0f}, 45.0f);
                Engine::Renderer::Renderer2D::DrawRotateQuad({-0.5f, -0.5f, 0.2f}, {1.0f, 1.0f}, 45.0f, m_CheckerBoardTexture, 1.0f, {0.8f, 1.0f, 0.3f, 1.0f});
            }
            Engine::Renderer::Renderer2D::EndScene();
        }
    }
    // InstrumentorProfilingEnd();
}
void Forest2D::OnImGuiRender()
{
    ENGINE_PROFILING_FUNC();
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

    ImGui::Text("Renderer2D Stats:");
    auto stats = Engine::Renderer::Renderer2D::GetStats();
    ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    ImGui::Text("Quad Count: %d", stats.QuadCount);
    ImGui::Text("Vertex Count: %d", stats.GetTotalVertexCount());
    ImGui::Text("Index Count: %d", stats.GetTotalIndexCount());

    int maxQuads = Engine::Renderer::Renderer2D::GetMaxQuads();
    ImGui::Text("Max Quads: %d", maxQuads);
    ImGui::InputInt("Max Quads", &maxQuads);
    ImGui::End();

    Engine::Renderer::Renderer2D::SetMaxQuads(maxQuads);
}

bool Forest2D::OnEvent(Engine::Event::Event &event)
{
    m_CameraController.OnEvent(event);
    return false;
}
