#pragma once
#include "Camera.h"

namespace Engine::Renderer
{
    class PerspectiveCamera : public Camera
    {
    public:
        PerspectiveCamera(float fovDegrees, float aspectRatio, float nearClip, float farClip);

        virtual void SetProjection(float fovDegrees, float aspectRatio, float nearClip, float farClip);

        virtual void SetPosition(const glm::vec3 &position) override;
        virtual void SetRotationDegrees(const glm::vec3 &rotation) override; // In degrees

        float GetFOVDegrees() const { return m_FOVDegrees; }
        float GetAspectRatio() const { return m_AspectRatio; }
        float GetNearClip() const { return m_NearClip; }
        float GetFarClip() const { return m_FarClip; }

    private:
        void RecalculateViewMatrix();

    private:
        float m_FOVDegrees;
        float m_AspectRatio;
        float m_NearClip;
        float m_FarClip;
    };
}