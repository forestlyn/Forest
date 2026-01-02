#pragma once
#include "Engine.h"
#include <glm/glm.hpp>
#include "Engine/Renderer/FrameBuffer.h"
#include "Panels/SceneHierarchyPanel.h"

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

    private:
        glm::vec4 m_SquareColor = {0.2f, 0.3f, 0.8f, 1.0f};

        Engine::Ref<Engine::Renderer::FrameBuffer> m_FrameBuffer;

        bool m_FocusScene = false, m_HoverScene = false;
        glm::ivec2 m_SceneViewportSize = {0, 0};

        Engine::Ref<Engine::Scene> m_Scene;
        Engine::Entity m_SquareEntity;
        Engine::Entity m_MainCameraEntity;
        Engine::Entity m_SecondCameraEntity;
        bool m_UseMainCamera = true;

        SceneHierarchyPanel m_SceneHierarchyPanel;
    };
}