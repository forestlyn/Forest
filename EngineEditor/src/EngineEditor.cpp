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
        : Layer(name)
    {
        // Initialize FrameBuffer
        Engine::Renderer::FrameBufferSpecification fbSpec;
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_FrameBuffer = Engine::Renderer::FrameBuffer::Create(fbSpec);

        m_Scene = Engine::CreateRef<Engine::Scene>();

        m_SquareEntity = m_Scene->CreateEntity("Square");
        m_SquareEntity.AddComponent<Engine::SpriteComponent>(glm::vec4{0.8f, 0.2f, 0.3f, 1.0f});
        m_SquareEntity.AddComponent<Engine::TransformComponent>(glm::vec3{2.0f, 0.0f, 0.0f});

        m_MainCameraEntity = m_Scene->CreateEntity("Main Camera");
        auto &cameraComponent = m_MainCameraEntity.AddComponent<Engine::CameraComponent>();
        cameraComponent.Camera = Engine::CreateRef<Engine::SceneCamera>();
        cameraComponent.Primary = true;
        m_MainCameraEntity.AddComponent<Engine::TransformComponent>();

        m_SecondCameraEntity = m_Scene->CreateEntity("Second Camera");
        auto &secondCameraComponent = m_SecondCameraEntity.AddComponent<Engine::CameraComponent>();
        secondCameraComponent.Camera = Engine::CreateRef<Engine::SceneCamera>();
        secondCameraComponent.Primary = false;
        m_SecondCameraEntity.AddComponent<Engine::TransformComponent>(glm::vec3{0.0f, 0.0f, 0.0f});

        class SquareScript : public Engine::ScriptEntity
        {
        public:
            SquareScript() = default;
            virtual void OnCreate() override
            {
                ENGINE_INFO("SquareScript::OnCreate");
            }

            virtual void OnUpdate(Engine::Core::Timestep ts) override
            {
                // ENGINE_INFO("SquareScript::OnUpdate: {0}", ts.GetSeconds());
                auto &transform = GetComponent<Engine::TransformComponent>();
                float speed = 2.0f;
                auto position = transform.GetPosition();
                position.x += speed * ts.GetSeconds();
                if (position.x > 5.0f)
                    position.x = -5.0f;
                transform.SetPosition(position);
            }

            virtual void OnDestroy() override
            {
                ENGINE_INFO("SquareScript::OnDestroy");
            }
        };

        auto &nativeScript = m_SquareEntity.AddComponent<Engine::NativeScriptComponent>();
        nativeScript.Bind<SquareScript>();

        m_SceneHierarchyPanel = SceneHierarchyPanel(m_Scene);
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

            // m_ParticleSystem->OnUpdate(timestep);
            {
                ENGINE_PROFILING_SCOPE("EngineEditor::PreRenderer");

                m_FrameBuffer->Bind();

                Engine::Renderer::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
                Engine::Renderer::RenderCommand::Clear();
            }

            {
                Timer_Profiling("EngineEditor::Renderer2D Scene");

                m_Scene->OnUpdate(timestep);

                m_FrameBuffer->Unbind();
            }
        }
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
            m_Scene->SetViewportSize((uint32_t)m_SceneViewportSize.x, (uint32_t)m_SceneViewportSize.y);
        }
        uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
        auto m_Specs = m_FrameBuffer->GetSpecification();
        ImGui::Image((void *)(uint64_t)textureID, ImVec2{(float)m_Specs.Width, (float)m_Specs.Height}, ImVec2{0, 1}, ImVec2{1, 0});

        ImGui::End();
        ImGui::PopStyleVar();

        Engine::Renderer::Renderer2D::SetMaxQuads(maxQuads);

        m_SceneHierarchyPanel.OnImGuiRender();
    }

    bool EngineEditor::OnEvent(Engine::Event::Event &event)
    {
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