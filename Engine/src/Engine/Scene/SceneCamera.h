#pragma once
#include "Engine/Core/Core.h"
#include <glm/glm.hpp>
#include "Engine/Renderer/Camera/OrthographicCamera.h"
#include "Engine/Renderer/Camera/PerspectiveCamera.h"

namespace Engine
{
    class SceneCamera
    {
    public:
        enum class ProjectionType
        {
            Orthographic = 0,
            Perspective = 1
        };

        SceneCamera();
        ~SceneCamera() = default;

        glm::mat4 GetProjectionMatrix() const { return m_Camera->GetProjectionMatrix(); }

        ProjectionType GetProjectionType() const { return m_ProjectionType; }
        void SetProjectionType(ProjectionType type);

        float GetAspectRatio() const { return m_AspectRatio; }
        void SetAspectRatio(float aspectRatio);

        // OrthographicCamera
        float GetOrthographicSize() const { return m_OrthographicSize; }
        void SetOrthographicSize(float size);

        float GetOrthographicNearClip() const { return m_OrthographicNear; }
        void SetOrthographicNearClip(float nearClip);

        float GetOrthographicFarClip() const { return m_OrthographicFar; }
        void SetOrthographicFarClip(float farClip);

        // PerspectiveCamera
        float GetPerspectiveFOV() const { return m_PerspectiveFOV; }
        void SetPerspectiveFOV(float fov);

        float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
        void SetPerspectiveNearClip(float nearClip);

        float GetPerspectiveFarClip() const { return m_PerspectiveFar; }
        void SetPerspectiveFarClip(float farClip);

    private:
        ProjectionType m_ProjectionType = ProjectionType::Orthographic;

        // Orthographic
        Ref<Renderer::OrthographicCamera> m_OrthographicCamera;
        float m_OrthographicSize = 10.0f;
        float m_OrthographicNear = -1.0f;
        float m_OrthographicFar = 1.0f;

        // Perspective
        Ref<Renderer::PerspectiveCamera> m_PerspectiveCamera;
        float m_PerspectiveFOV = 45.0f;
        float m_PerspectiveNear = 0.1f;
        float m_PerspectiveFar = 1000.0f;

        float m_AspectRatio = 16.0f / 9.0f;

        Ref<Renderer::Camera> m_Camera;
    };
}