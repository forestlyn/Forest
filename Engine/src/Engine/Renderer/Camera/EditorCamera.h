#pragma once
#include "Engine/Renderer/Camera/OrthographicCamera.h"
#include "Engine/Core/Timestep.h"
namespace Engine::Renderer
{
    class EditorCamera
    {
    public:
        EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);
        ~EditorCamera();

        void OnUpdate(Core::Timestep ts);
        void OnEvent(Event::Event &e);

        glm::mat4 GetViewMatrix() const { return m_ViewMatrix; }
        glm::mat4 GetProjectionMatrix() const { return m_ProjectionMatrix; }
        glm::mat4 GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

        void SetViewportSize(int width, int height)
        {
            m_AspectRatio = (float)width / (float)height;
            m_ViewportWidth = (float)width;
            m_ViewportHeight = (float)height;
            UpdateProjection();
        }

        inline float GetDistance() const { return m_Distance; }
        inline void SetDistance(float distance) { m_Distance = distance; }

        glm::vec3 GetUpDirection() const;
        glm::vec3 GetRightDirection() const;
        glm::vec3 GetForwardDirection() const;
        const glm::vec3 &GetPosition() const { return m_Position; }
        glm::quat GetOrientation() const;

        float GetPitch() const { return m_Pitch; }
        float GetYaw() const { return m_Yaw; }

    private:
        void UpdateProjection();
        void UpdateView();

        bool OnMouseScroll(Event::MouseScrolledEvent &e);

        void MousePan(const glm::vec2 &delta);
        void MouseRotate(const glm::vec2 &delta);
        void MouseZoom(float delta);

        glm::vec3 CalculatePosition() const;

        std::pair<float, float> PanSpeed() const;
        float RotationSpeed() const;
        float ZoomSpeed() const;

    private:
        float m_FOV;
        float m_AspectRatio;
        float m_NearClip;
        float m_FarClip;

        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ViewProjectionMatrix;

        glm::vec3 m_Position = {0.0f, 0.0f, 20.0f};
        glm::vec3 m_FocalPoint = {0.0f, 0.0f, 0.0f};

        glm::vec2 m_InitialMousePosition = {0.0f, 0.0f};

        float m_Distance = 10.0f;
        float m_Pitch = 0.0f, m_Yaw = 0.0f;

        float m_ViewportWidth = 1280, m_ViewportHeight = 720;
    };
} // namespace Engine::Renderer