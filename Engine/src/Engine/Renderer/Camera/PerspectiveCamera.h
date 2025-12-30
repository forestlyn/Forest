#pragma once
#include "Camera.h"

namespace Engine::Renderer
{
    class PerspectiveCamera : public Camera
    {
    public:
        PerspectiveCamera(float fovDegrees, float aspectRatio, float nearClip, float farClip);

        float GetFOVDegrees() const { return m_FOVDegrees; }
        float GetAspectRatio() const { return m_AspectRatio; }
        float GetNearClip() const { return m_NearClip; }
        float GetFarClip() const { return m_FarClip; }

        void SetFOVDegrees(float fovDegrees)
        {
            m_FOVDegrees = fovDegrees;
            RecalculateProjectMatrix();
        }
        void SetAspectRatio(float aspectRatio)
        {
            m_AspectRatio = aspectRatio;
            RecalculateProjectMatrix();
        }
        void SetNearClip(float nearClip)
        {
            m_NearClip = nearClip;
            RecalculateProjectMatrix();
        }
        void SetFarClip(float farClip)
        {
            m_FarClip = farClip;
            RecalculateProjectMatrix();
        }

    private:
        void RecalculateProjectMatrix();

    private:
        float m_FOVDegrees;
        float m_AspectRatio;
        float m_NearClip;
        float m_FarClip;
    };
}