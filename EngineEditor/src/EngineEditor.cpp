#include "EngineEditor.h"
#include "imgui.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Engine/Profile/ProfileLayer.h"
#include "Engine/Profile/Instrumentor.h"
namespace EngineEditor
{

    EngineEditor::EngineEditor(const std::string &name)
        : Layer(name), m_CameraController(Engine::OrthographicCameraController(16.0f / 9.0f, 1.0f, true))
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

        // Initialize FrameBuffer
        Engine::Renderer::FrameBufferSpecification fbSpec;
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_FrameBuffer = Engine::Renderer::FrameBuffer::Create(fbSpec);
    }

    EngineEditor::~EngineEditor()
    {
    }

    void EngineEditor::OnAttach()
    {
        LOG_INFO("EngineEditor Layer attached!");
    }

    void EngineEditor::OnDetach()
    {
        LOG_INFO("EngineEditor Layer detached!");
    }

    void EngineEditor::OnUpdate(Engine::Core::Timestep timestep)
    {
        {
            ENGINE_PROFILING_FUNC();

            // if (Engine::Core::Input::IsMouseButtonPressed(FOREST_MOUSE_BUTTON_LEFT))
            // {
            //     auto [x, y] = Engine::Core::Input::GetMousePosition();
            //     glm::vec2 worldPos = m_CameraController.GetCamera().ScreenToWorld({x, y});
            //     m_ParticleTemplate.Position = glm::vec3(worldPos.x, worldPos.y, 0.0f);
            //     // ENGINE_INFO("Left Mouse Button Pressed - Emitting Particle: Screen Pos: ({}, {}), World Pos: ({}, {})", x, y, worldPos.x, worldPos.y);
            //     m_ParticleSystem->Emit(m_ParticleTemplate);
            // }

            // Update and Render Particle System

            if (m_FocusScene)
                m_CameraController.OnUpdate(timestep);

            // m_ParticleSystem->OnUpdate(timestep);
            {
                ENGINE_PROFILING_SCOPE("EngineEditor::PreRenderer");

                m_FrameBuffer->Bind();

                Engine::Renderer::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
                Engine::Renderer::RenderCommand::Clear();
            }

            {
                Timer_Profiling("EngineEditor::Renderer2D Scene");

                Engine::Renderer::Renderer2D::BeginScene(m_CameraController.GetCamera());
                Engine::Renderer::Renderer2D::ResetStats();
                {
                    ENGINE_PROFILING_SCOPE("Renderer2D::DrawQuad");
                    // m_ParticleSystem->OnRender();

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

                m_FrameBuffer->Unbind();
            }
        }
        // InstrumentorProfilingEnd();
    }
    void EngineEditor::OnImGuiRender()
    {
        OpenDockSpace();

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

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Scene");

        m_FocusScene = ImGui::IsWindowFocused();
        m_HoverScene = ImGui::IsWindowHovered();

        Engine::Core::Application::Get().GetImGuiLayer().BlockEvents(!(m_FocusScene || m_HoverScene));
        // ENGINE_INFO("m_FocusScene:{},m_HoverScene:{}", m_FocusScene, m_HoverScene);

        auto regionAvailSize = ImGui::GetContentRegionAvail();
        if (m_SceneViewportSize.x != (int)regionAvailSize.x || m_SceneViewportSize.y != (int)regionAvailSize.y)
        {
            m_SceneViewportSize.x = (int)regionAvailSize.x;
            m_SceneViewportSize.y = (int)regionAvailSize.y;
            m_FrameBuffer->Resize(m_SceneViewportSize.x, m_SceneViewportSize.y);
            m_CameraController.OnResize((float)m_SceneViewportSize.x, (float)m_SceneViewportSize.y);
        }
        uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
        auto m_Specs = m_FrameBuffer->GetSpecification();
        ImGui::Image((void *)textureID, ImVec2{(float)m_Specs.Width, (float)m_Specs.Height}, ImVec2{0, 1}, ImVec2{1, 0});
        ImGui::End();
        ImGui::PopStyleVar();

        Engine::Renderer::Renderer2D::SetMaxQuads(maxQuads);
    }

    bool EngineEditor::OnEvent(Engine::Event::Event &event)
    {
        m_CameraController.OnEvent(event);
        return false;
    }

    void EngineEditor::OpenDockSpace()
    {
        ENGINE_PROFILING_FUNC();

        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        bool p_open = true;
        ImGui::Begin("DockSpace Demo", &p_open, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        // REMINDER: THIS IS A DEMO FOR ADVANCED USAGE OF DockSpace()!
        // MOST REGULAR APPLICATIONS WILL SIMPLY WANT TO CALL DockSpaceOverViewport(). READ COMMENTS ABOVE.
        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        ImGui::End();
    }
}