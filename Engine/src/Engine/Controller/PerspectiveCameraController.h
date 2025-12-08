#pragma once
#include "Engine/Renderer/Camera/PerspectiveCamera.h"
#include "Engine/Core/Timestep.h"
#include "Engine/Events/WindowEvent.h"
#include "Engine/Events/MouseEvent.h"
#include <glm/glm.hpp>
namespace Engine
{
    class PerspectiveCameraController
    {
    public:
        PerspectiveCameraController(float fov, float aspectRatio, glm::bvec3 rotation = {false, false, false});

        Renderer::PerspectiveCamera &GetCamera() { return m_Camera; }
        const Renderer::PerspectiveCamera &GetCamera() const { return m_Camera; }

        void OnUpdate(Core::Timestep timestep);
        void OnEvent(Event::Event &event);

    private:
        bool OnWindowResize(Event::WindowResizeEvent &event);
        bool OnScroll(Event::MouseScrolledEvent &event);

    private:
        float m_AspectRatio;
        float m_FOV;
        Renderer::PerspectiveCamera m_Camera;
        glm::bvec3 m_Rotation;

        float m_CameraMoveSpeed = 1.f;
        float m_CameraRotationSpeed = 20.0f;
        glm::vec3 m_CameraPosition = {0.0f, 0.0f, 0.0f};
        glm::vec3 m_CameraRotation = {0.0f, 0.0f, 0.0f};
    };
}