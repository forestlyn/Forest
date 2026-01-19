#include "EngineEditor.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Engine/Profile/ProfileLayer.h"
#include "Engine/Profile/Instrumentor.h"
#include "Engine/Serialization/SceneSerialize.h"
#include "Engine/Utils/FilePlatformUtils.h"

namespace EngineEditor
{

    EngineEditor::EngineEditor(const std::string &name)
        : Layer(name), m_EditorCamera(Engine::Renderer::EditorCamera(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f))
    {
        // Initialize FrameBuffer
        Engine::Renderer::FrameBufferSpecification fbSpec;
        fbSpec.Attachments = {Engine::Renderer::TextureFormat::RGBA8,
                              Engine::Renderer::TextureFormat::DEPTH24STENCIL8,
                              Engine::Renderer::TextureFormat::RED_INTEGER_32};
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_FrameBuffer = Engine::Renderer::FrameBuffer::Create(fbSpec);

        m_ActiveScene = Engine::CreateRef<Engine::Scene>();
        m_SceneHierarchyPanel = SceneHierarchyPanel(m_ActiveScene);
        m_ContentBrowserPanel = ContentBrowserPanel();

        m_PlayIcon = Engine::Renderer::Texture2D::Create("assets/textures/icon/play_icon.png");
        m_StopIcon = Engine::Renderer::Texture2D::Create("assets/textures/icon/stop_icon.png");
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
            m_EditorCamera.OnUpdate(timestep);
            // m_ParticleSystem->OnUpdate(timestep);
            {
                ENGINE_PROFILING_SCOPE("EngineEditor::PreRenderer");

                m_FrameBuffer->Bind();

                Engine::Renderer::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
                Engine::Renderer::RenderCommand::Clear();

                m_FrameBuffer->ClearAttachment(1, -1); // Clear entityID attachment
            }

            {
                Timer_Profiling("EngineEditor::Renderer2D Scene");

                if (m_SceneState == SceneState::Play) // Runtime update
                {
                    m_ActiveScene->OnUpdateRuntime(timestep);
                }
                else // Editor update
                {
                    m_ActiveScene->OnUpdateEditor(timestep, m_EditorCamera);
                }

                auto mousePos = ImGui::GetMousePos();

                if (mousePos.x >= m_SceneViewportBounds[0].x && mousePos.x <= m_SceneViewportBounds[1].x &&
                    mousePos.y >= m_SceneViewportBounds[0].y && mousePos.y <= m_SceneViewportBounds[1].y)
                {
                    int mouseX = (int)(mousePos.x - m_SceneViewportBounds[0].x);
                    int mouseY = (int)(mousePos.y - m_SceneViewportBounds[0].y);
                    mouseY = m_SceneViewportSize.y - mouseY; // Flip Y coordinate

                    int pixelData = m_FrameBuffer->GetPixelData(1, mouseX, mouseY);
                    if (pixelData != -1)
                    {
                        m_HoveredEntity = Engine::Entity((entt::entity)pixelData, m_ActiveScene.get());
                    }
                    else
                    {
                        m_HoveredEntity = {};
                    }
                }
                else
                {
                    m_HoveredEntity = {};
                }

                m_FrameBuffer->Unbind();
            }
        }
    }
    void EngineEditor::OnImGuiRender()
    {
        OpenDockSpace();

        ENGINE_PROFILING_FUNC();
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Scene Ctrl+N"))
                {
                    NewScene();
                }
                if (ImGui::MenuItem("Load Scene Ctrl+Shift+O"))
                {
                    LoadScene();
                }
                if (ImGui::MenuItem("Save Scene Ctrl+S"))
                {
                    SaveScene(m_EditorScenePath);
                }
                if (ImGui::MenuItem("Save Scene Ctrl+Shift+S"))
                {
                    SaveSceneAs();
                }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

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
            m_ActiveScene->SetViewportSize((uint32_t)m_SceneViewportSize.x, (uint32_t)m_SceneViewportSize.y);
            m_EditorCamera.SetViewportSize(m_SceneViewportSize.x, m_SceneViewportSize.y);
        }
        uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID(0);
        auto m_Specs = m_FrameBuffer->GetSpecification();
        ImGui::Image((void *)(uint64_t)textureID, ImVec2{(float)m_Specs.Width, (float)m_Specs.Height}, ImVec2{0, 1}, ImVec2{1, 0});
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const wchar_t *path = (const wchar_t *)payload->Data;
                std::filesystem::path filepath(path);
                ENGINE_INFO("Accept payload: {}", filepath.string());
                LoadScene(filepath);
            }
            ImGui::EndDragDropTarget();
        }

        // Gizmos
        {
            Engine::Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
            if (selectedEntity && ImGuizmo_operation != -1)
            {
                ImGuizmo::SetDrawlist();
                float windowWidth = (float)ImGui::GetWindowWidth();
                float windowHeight = (float)ImGui::GetWindowHeight();
                ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

                auto &selectedTc = selectedEntity.GetComponent<Engine::TransformComponent>();
                glm::mat4 selectedTransform = selectedTc.GetTransform();

                ImGuizmo::OPERATION guizmoOperation = static_cast<ImGuizmo::OPERATION>(ImGuizmo_operation);
                ImGuizmo::MODE guizmoMode = ImGuizmo::MODE::LOCAL;

                bool isOrthographic = false; // TODO: get from camera settings
                ImGuizmo::SetOrthographic(isOrthographic);

                float tcsnap[3] = {0.5f, 0.5f, 0.5f};
                float rotationSnap = 45.0f;
                float scaleSnap = 0.5f;
                float *snapPtr = nullptr;
                if (Engine::Core::Input::IsKeyPressed(FOREST_KEY_LEFT_CONTROL))
                {
                    switch (guizmoOperation)
                    {
                    case ImGuizmo::OPERATION::TRANSLATE:
                        snapPtr = tcsnap;
                        break;
                    case ImGuizmo::OPERATION::ROTATE:
                        snapPtr = &rotationSnap;
                        break;
                    case ImGuizmo::OPERATION::SCALE:
                        snapPtr = &scaleSnap;
                        break;
                    default:
                        snapPtr = nullptr;
                        break;
                    }
                }

                ImGuizmo::Manipulate(glm::value_ptr(m_EditorCamera.GetViewMatrix()),
                                     glm::value_ptr(m_EditorCamera.GetProjectionMatrix()),
                                     guizmoOperation,
                                     guizmoMode,
                                     glm::value_ptr(selectedTransform), 0,
                                     snapPtr);

                if (ImGuizmo::IsUsing())
                {
                    glm::vec3 translation, rotation, scale;
                    Engine::Math::DecomposeTransform(selectedTransform, translation, rotation, scale);

                    auto deltaRotation = rotation - selectedTc.GetRotation();

                    rotation = selectedTc.GetRotation() + deltaRotation;

                    selectedTc.SetPosition(translation);
                    selectedTc.SetRotation(rotation);
                    selectedTc.SetScale(scale);
                }
            }
        }

        // Set Scene Viewport bounds
        auto windowPos = ImGui::GetWindowPos();
        auto viewportMin = ImGui::GetWindowContentRegionMin();
        auto viewportMax = ImGui::GetWindowContentRegionMax();

        m_SceneViewportBounds[0] = {windowPos.x + viewportMin.x, windowPos.y + viewportMin.y};
        m_SceneViewportBounds[1] = {windowPos.x + viewportMax.x, windowPos.y + viewportMax.y};

        ImGui::End();
        ImGui::PopStyleVar();

        Engine::Renderer::Renderer2D::SetMaxQuads(maxQuads);

        UIToolbar();

        m_SceneHierarchyPanel.OnImGuiRender();
        m_ContentBrowserPanel.OnImGuiRender();
    }

    bool EngineEditor::OnEvent(Engine::Event::Event &event)
    {
        Engine::Event::EventDispatcher dispatcher(event);
        event.Handled |= dispatcher.Dispatch<Engine::Event::KeyPressedEvent>(BIND_EVENT_FN(EngineEditor::KeyPressedEventHandler));
        event.Handled |= dispatcher.Dispatch<Engine::Event::MouseButtonPressedEvent>(BIND_EVENT_FN(EngineEditor::MousePressedEventHandler));
        return false;
    }

    bool EngineEditor::KeyPressedEventHandler(Engine::Event::KeyPressedEvent &event)
    {
        ENGINE_INFO("KeyPressedEvent: {}", event.ToString());
        bool isCtrlPressed = Engine::Core::Input::IsKeyPressed(FOREST_KEY_LEFT_CONTROL) || Engine::Core::Input::IsKeyPressed(FOREST_KEY_RIGHT_CONTROL);
        bool isShiftPressed = Engine::Core::Input::IsKeyPressed(FOREST_KEY_LEFT_SHIFT) || Engine::Core::Input::IsKeyPressed(FOREST_KEY_RIGHT_SHIFT);

        switch (event.GetKeyCode())
        {
        case FOREST_KEY_N:
            if (isCtrlPressed)
            {
                NewScene();
                return true;
            }
        case FOREST_KEY_O:
            if (isCtrlPressed && isShiftPressed)
            {
                LoadScene();
                return true;
            }
        case FOREST_KEY_S:
            if (isCtrlPressed && isShiftPressed)
            {
                SaveSceneAs();
                return true;
            }
            else if (isCtrlPressed)
            {
                SaveScene(m_EditorScenePath);
                return true;
            }
        case FOREST_KEY_D:
            if (isCtrlPressed)
            {
                ENGINE_INFO("Duplicate Entity Shortcut Pressed");
                DuplicateEntity(m_SceneHierarchyPanel.GetSelectedEntity());
                return true;
            }
        case FOREST_KEY_Q:
            if (ImGuizmo::IsUsing())
                return true;
            ImGuizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
            break;
        case FOREST_KEY_W:
            if (ImGuizmo::IsUsing())
                return true;
            ImGuizmo_operation = ImGuizmo::OPERATION::ROTATE;
            break;
        case FOREST_KEY_E:
            if (ImGuizmo::IsUsing())
                return true;
            ImGuizmo_operation = ImGuizmo::OPERATION::SCALE;
            break;
        default:
            break;
        }
        return false;
    }

    bool EngineEditor::MousePressedEventHandler(Engine::Event::MouseButtonPressedEvent &event)
    {
        switch (event.GetMouseButton())
        {
        case FOREST_MOUSE_BUTTON_LEFT:
            if (CanPickEntity())
            {
                m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
                return true;
            }
            break;

        default:
            break;
        }
        return false;
    }

    bool EngineEditor::CanPickEntity()
    {
        return m_FocusScene && !ImGui::IsKeyPressed(ImGuiKey_LeftAlt) && !ImGuizmo::IsOver();
    }

    void EngineEditor::DuplicateEntity(Engine::Entity entity)
    {
        if (!entity)
            return;
        m_ActiveScene->DuplicateEntity(entity);
    }

    void EngineEditor::UIToolbar()
    {
        ImGui::Begin("##UIToolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

        float windowWidth = ImGui::GetWindowWidth();
        float buttonSize = 25.0f;
        float center = windowWidth * 0.5f;
        float offset = buttonSize * 0.5f;

        ImGui::SetCursorPosX(center - offset - 80.0f);

        if (m_SceneState == SceneState::Edit)
        {
            if (ImGui::ImageButton("##PlayScene", ImTextureRef((void *)m_PlayIcon->GetRendererID()), ImVec2(buttonSize, buttonSize)))
            {
                PlayScene();
            }
        }
        else if (m_SceneState == SceneState::Play)
        {
            if (ImGui::ImageButton("##StopScene", ImTextureRef((void *)m_StopIcon->GetRendererID()), ImVec2(buttonSize, buttonSize)))
            {
                StopScene();
            }
        }
        ImGui::End();
    }

    void EngineEditor::PlayScene()
    {
        m_SceneState = SceneState::Play;
        m_ActiveScene = Engine::Scene::Copy(m_EditorScene);
        m_RuntimeScene = m_ActiveScene;
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
        m_ActiveScene->OnRuntimeStart();
    }

    void EngineEditor::StopScene()
    {
        m_SceneState = SceneState::Edit;
        m_ActiveScene->OnRuntimeStop();

        m_ActiveScene = m_EditorScene;
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
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

    void EngineEditor::NewScene()
    {
        m_EditorScene = Engine::CreateRef<Engine::Scene>();
        m_ActiveScene = m_EditorScene;
        m_EditorScenePath.clear();
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
        m_ActiveScene->SetViewportSize((uint32_t)m_SceneViewportSize.x, (uint32_t)m_SceneViewportSize.y);
    }

    void EngineEditor::LoadScene()
    {
        std::string scenePath = Engine::FileDialog::OpenFileDialog("Scene Files\0*.scene\0All Files\0*.*\0");
        LoadScene(scenePath);
    }

    void EngineEditor::LoadScene(std::filesystem::path path)
    {
        if (!path.empty())
        {
            std::string ext = path.extension().string();
            if (ext != ".scene")
            {
                ENGINE_ERROR("Could not load scene file '{0}' - not a .scene file!", path.string());
                return;
            }
            m_EditorScene = Engine::CreateRef<Engine::Scene>();
            m_ActiveScene = m_EditorScene;
            m_EditorScenePath = path;
            Engine::Serialization::SceneSerialize sceneSerialize(m_ActiveScene);
            sceneSerialize.Deserialize(path.string());
            m_SceneHierarchyPanel.SetContext(m_ActiveScene);
            m_ActiveScene->SetViewportSize((uint32_t)m_SceneViewportSize.x, (uint32_t)m_SceneViewportSize.y);
        }
    }

    void EngineEditor::SaveScene(std::filesystem::path path)
    {
        if (path.empty())
        {
            SaveSceneAs();
            return;
        }
        Engine::Serialization::SceneSerialize sceneSerialize(m_ActiveScene);
        sceneSerialize.Serialize(path.string());
    }

    void EngineEditor::SaveSceneAs()
    {
        std::string scenePath = Engine::FileDialog::SaveFileDialog("Scene Files\0*.scene\0All Files\0*.*\0");
        if (!scenePath.empty())
        {
            Engine::Serialization::SceneSerialize sceneSerialize(m_ActiveScene);
            sceneSerialize.Serialize(scenePath);
            m_EditorScenePath = scenePath;
            return;
        }
    }
}