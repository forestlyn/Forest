#pragma once
#include "Engine/Renderer/Camera/PerspectiveCamera.h"
#include "Engine/Core/Timestep.h"
#include "Engine/Events/WindowEvent.h"
#include "Engine/Events/MouseEvent.h"
#include <glm/glm.hpp>
#include "Engine/Scene/Entity.h"
namespace Engine
{
    class PerspectiveCameraController
    {
    public:
        PerspectiveCameraController() = default;
        PerspectiveCameraController(Entity *camera, glm::bvec3 rotation = {false, false, false});

        Renderer::PerspectiveCamera &GetCamera() { return *m_PerspectiveCamera; }
        const Renderer::PerspectiveCamera &GetCamera() const { return *m_PerspectiveCamera; }
        void SetCameraEntity(Entity *camera);

        void OnUpdate(Core::Timestep timestep);
        void OnEvent(Event::Event &event);
        glm::vec2 ScreenToWorld(const glm::vec2 &screenPos, const glm::vec2 &viewportSize);

        glm::mat4 GetViewProjectionMatrix();

    private:
        bool OnWindowResize(Event::WindowResizeEvent &event);
        bool OnScroll(Event::MouseScrolledEvent &event);

    private:
        Entity *m_CameraEntity;

        TransformComponent *m_CameraTransform;
        CameraComponent *m_CameraComponent;

        Renderer::PerspectiveCamera *m_PerspectiveCamera;

        glm::bvec3 m_Rotation;

        float m_CameraMoveSpeed = 1.f;
        float m_CameraRotationSpeed = 20.0f;
        glm::vec3 m_CameraPosition = {0.0f, 0.0f, 0.0f};
        glm::vec3 m_CameraRotation = {0.0f, 0.0f, 0.0f};
    };
}