#pragma once
#include "Engine/Renderer/Camera/OrthographicCamera.h"
#include "Engine/Core/Timestep.h"
#include "Engine/Events/WindowEvent.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Scene/Entity.h"

namespace Engine
{
    class OrthographicCameraController
    {
    public:
        OrthographicCameraController() = default;
        OrthographicCameraController(Entity *camera, bool rotation = false);

        Renderer::OrthographicCamera &GetCamera() { return *m_OrthoCamera; }
        const Renderer::OrthographicCamera &GetCamera() const { return *m_OrthoCamera; }

        void SetCameraEntity(Entity *camera);
        void OnUpdate(Core::Timestep timestep);
        void OnEvent(Event::Event &event);

        void OnResize(float width, float height);
        glm::vec2 ScreenToWorld(const glm::vec2 &screenPos, const glm::vec2 &viewportSize);
        glm::mat4 GetViewProjectionMatrix();

    private:
        bool OnWindowResize(Event::WindowResizeEvent &event);
        bool OnScroll(Event::MouseScrolledEvent &event);

    private:
        Entity *m_CameraEntity;
        bool m_Rotation = false;

        Renderer::OrthographicCamera *m_OrthoCamera;
        TransformComponent *m_CameraTransform;
        CameraComponent *m_CameraComponent;

        float m_CameraMoveSpeed = 1.f;
        float m_CameraRotationSpeed = 20.0f;
        glm::vec3 m_CameraPosition = {0.0f, 0.0f, 0.0f};
        glm::vec3 m_CameraRotation = {0.0f, 0.0f, 0.0f};
    };
}