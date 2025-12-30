#pragma once
#include "Camera.h"
namespace Engine::Renderer
{
    class OrthographicCamera : public Camera
    {
    public:
        OrthographicCamera(float left, float right, float bottom, float top);
        OrthographicCamera(float aspectRatio, float zoomLevel);

        float GetAspectRatio() const { return m_AspectRatio; }
        float GetZoomLevel() const { return m_ZoomLevel; }
        float GetNearClip() const { return m_NearClip; }
        float GetFarClip() const { return m_FarClip; }

        void SetAspectRatio(float aspectRatio)
        {
            m_AspectRatio = aspectRatio;
            RecalculateProjectMatrix();
        }
        void SetZoomLevel(float zoomLevel)
        {
            m_ZoomLevel = zoomLevel;
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
        float m_AspectRatio;
        float m_ZoomLevel;
        float m_NearClip = -100.0f;
        float m_FarClip = 100.0f;
    };
}