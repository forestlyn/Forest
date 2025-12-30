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
    : Layer(name)
{
    m_CheckerBoardTexture = Engine::Renderer::Texture2D::Create("assets/textures/Checkerboard.png");
    m_SpriteSheetTexture = Engine::Renderer::Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png");
    m_TreeSubTexture = Engine::Renderer::SubTexture2D::CreateFromCoords(m_SpriteSheetTexture, {128.0f, 128.0f}, {1, 1}, {1, 2});
    m_Upstairs = Engine::Renderer::SubTexture2D::CreateFromCoords(m_SpriteSheetTexture, {128.0f, 128.0f}, {7, 6});

    // Initialize Particle System
    m_ParticleSystem = std::make_unique<ParticleSystem>();
    m_ParticleTemplate = ParticleProperties();
    m_ParticleTemplate.Velocity = {0.0f, 0.0f, 0.0f};
    m_ParticleTemplate.VelocityVariation = {0.5f, 0.5f, 0.0f};
    m_ParticleTemplate.ColorBegin = {1.0f, 0.5f, 0.2f, 1.0f};
    m_ParticleTemplate.ColorEnd = {0.2f, 0.3f, 0.8f, 0.0f};
    m_ParticleTemplate.SizeBegin = 0.1f;
    m_ParticleTemplate.SizeEnd = 0.01f;
    m_ParticleTemplate.SizeVariation = 0.05f;
    m_ParticleTemplate.LifeTime = 1.0f;

    m_Scene = Engine::CreateScope<Engine::Scene>();
    m_CameraEntity = m_Scene->CreateEntity("Camera");
    auto &cameraComponent = m_CameraEntity.AddComponent<Engine::CameraComponent>();
    cameraComponent.Camera = Engine::CreateRef<Engine::Renderer::OrthographicCamera>(-16.0f / 9.0f, 16.0f / 9.0f, -1.0f, 1.0f);
    cameraComponent.Primary = true;
    m_CameraEntity.AddComponent<Engine::TransformComponent>();

    m_CameraController.SetCameraEntity(&m_CameraEntity);
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

        if (Engine::Core::Input::IsMouseButtonPressed(FOREST_MOUSE_BUTTON_LEFT))
        {
            auto [x, y] = Engine::Core::Input::GetMousePosition();
            auto width = Engine::Core::Application::Get().GetWindowWidth();
            auto height = Engine::Core::Application::Get().GetWindowHeight();
            glm::vec2 worldPos = m_CameraController.ScreenToWorld({x, y}, {(float)width, (float)height});
            m_ParticleTemplate.Position = glm::vec3(worldPos.x, worldPos.y, 0.0f);
            // ENGINE_INFO("Left Mouse Button Pressed - Emitting Particle: Screen Pos: ({}, {}), World Pos: ({}, {})", x, y, worldPos.x, worldPos.y);
            m_ParticleSystem->Emit(m_ParticleTemplate);
        }

        // Update and Render Particle System
        m_CameraController.OnUpdate(timestep);
        m_Scene->OnUpdate(timestep);

        m_ParticleSystem->OnUpdate(timestep);
        {
            ENGINE_PROFILING_SCOPE("Forest2D::PreRenderer");
            Engine::Renderer::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
            Engine::Renderer::RenderCommand::Clear();
        }

        {
            Timer_Profiling("Forest2D::Renderer2D Scene");

            Engine::Renderer::Renderer2D::BeginScene(m_CameraController.GetViewProjectionMatrix());
            Engine::Renderer::Renderer2D::ResetStats();
            {
                ENGINE_PROFILING_SCOPE("Renderer2D::DrawQuad");
                m_ParticleSystem->OnRender();

                for (uint32_t i = 0; i < 100; i++)
                {
                    float x = (i % 100) * 0.11f - 5.0f;
                    float y = (i / 100) * 0.11f - 5.0f;
                    Engine::Renderer::Renderer2D::DrawQuad({x, y}, {0.1f, 0.1f}, {i % 255 / 255.0f, 0.3f, i / 255.0f, 1.0f});
                }

                Engine::Renderer::Renderer2D::DrawSubTextureQuad({-0.5f, -0.5f, 0.2f}, {1.0f, 2.0f}, m_TreeSubTexture);
                Engine::Renderer::Renderer2D::DrawSubTextureQuad({0.5f, -0.5f, 0.2f}, {1.0f, 1.0f}, m_Upstairs);
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
