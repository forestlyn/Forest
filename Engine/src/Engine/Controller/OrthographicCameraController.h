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
        OrthographicCameraController() = default;
        OrthographicCameraController(float aspectRatio, float zoomLevel = 1.0f, bool rotation = false);
        OrthographicCameraController(Renderer::OrthographicCamera *camera, bool rotation = false);

        Renderer::OrthographicCamera &GetCamera() { return *m_Camera; }
        const Renderer::OrthographicCamera &GetCamera() const { return *m_Camera; }
        void SetCamera(Renderer::OrthographicCamera *camera) { m_Camera = camera; }

        void OnUpdate(Core::Timestep timestep);
        void OnEvent(Event::Event &event);

        void OnResize(float width, float height);

    private:
        bool OnWindowResize(Event::WindowResizeEvent &event);
        bool OnScroll(Event::MouseScrolledEvent &event);

    private:
        Renderer::OrthographicCamera *m_Camera;
        bool m_Rotation = false;

        float m_CameraMoveSpeed = 1.f;
        float m_CameraRotationSpeed = 20.0f;
        glm::vec3 m_CameraPosition = {0.0f, 0.0f, 0.0f};
        glm::vec3 m_CameraRotation = {0.0f, 0.0f, 0.0f};
    };
}