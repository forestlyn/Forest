#pragma once
#include "Engine/Renderer/Camera/OrthographicCamera.h"
#include "Engine/Core/Timestep.h"
#include "Engine/Events/WindowEvent.h"
#include "Engine/Events/MouseEvent.h"

namespace Engine
{
    class OrthographicCameraController
    {
    public:
        OrthographicCameraController(float aspectRatio, float zoomLevel = 1.0f, bool rotation = false);

        Renderer::OrthographicCamera &GetCamera() { return m_Camera; }
        const Renderer::OrthographicCamera &GetCamera() const { return m_Camera; }

        void OnUpdate(Core::Timestep timestep);
        void OnEvent(Event::Event &event);

        void OnResize(float width, float height);

    private:
        bool OnWindowResize(Event::WindowResizeEvent &event);
        bool OnScroll(Event::MouseScrolledEvent &event);

    private:
        Renderer::OrthographicCamera m_Camera;
        float m_AspectRatio;
        float m_ZoomLevel;
        bool m_Rotation;

        float m_CameraMoveSpeed = 1.f;
        float m_CameraRotationSpeed = 20.0f;
        glm::vec3 m_CameraPosition = {0.0f, 0.0f, 0.0f};
        glm::vec3 m_CameraRotation = {0.0f, 0.0f, 0.0f};
    };
}