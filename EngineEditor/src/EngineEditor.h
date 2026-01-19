#pragma once
#include "Engine.h"
#include <glm/glm.hpp>
#include "Engine/Renderer/FrameBuffer.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Engine/Renderer/Camera/EditorCamera.h"
#include "Panels/ContentBrowserPanel.h"
#include "Engine/Renderer/Shader/Texture.h"
namespace EngineEditor
{

    class EngineEditor : public Engine::Core::Layer
    {
    public:
        EngineEditor(const std::string &name = "Forest2DLayer");
        virtual ~EngineEditor();

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnUpdate(Engine::Core::Timestep timestep) override;
        virtual void OnImGuiRender() override;
        virtual bool OnEvent(Engine::Event::Event &event) override;

    private:
        void OpenDockSpace();

        void NewScene();
        void SaveSceneAs();
        void SaveScene(std::filesystem::path path);
        void LoadScene();
        void LoadScene(std::filesystem::path path);

        bool KeyPressedEventHandler(Engine::Event::KeyPressedEvent &event);
        bool MousePressedEventHandler(Engine::Event::MouseButtonPressedEvent &event);
        bool CanPickEntity();

        void UIToolbar();

        void PlayScene();
        void StopScene();

    private:
        glm::vec4 m_SquareColor = {0.2f, 0.3f, 0.8f, 1.0f};

        Engine::Ref<Engine::Renderer::FrameBuffer> m_FrameBuffer;

        Engine::Ref<Engine::Renderer::Texture2D> m_PlayIcon;
        Engine::Ref<Engine::Renderer::Texture2D> m_StopIcon;

        bool m_FocusScene = false, m_HoverScene = false;
        glm::ivec2 m_SceneViewportSize = {0, 0};
        Engine::Entity m_HoveredEntity;

        Engine::Renderer::EditorCamera m_EditorCamera;
        Engine::Ref<Engine::Scene> m_ActiveScene;
        Engine::Ref<Engine::Scene> m_EditorScene;
        Engine::Ref<Engine::Scene> m_RuntimeScene;
        std::filesystem::path m_EditorScenePath;

        glm::vec2 m_SceneViewportBounds[2];

        int ImGuizmo_operation = -1;

        SceneHierarchyPanel m_SceneHierarchyPanel;
        ContentBrowserPanel m_ContentBrowserPanel;

        enum class SceneState
        {
            Edit = 0,
            Play = 1
        };

        SceneState m_SceneState = SceneState::Edit;
    };
}