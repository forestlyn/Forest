#pragma once
#include "Camera.h"
namespace Engine::Renderer
{
    class OrthographicCamera : public Camera
    {
    public:
        OrthographicCamera(float left, float right, float bottom, float top);
        OrthographicCamera(float aspectRatio, float zoomLevel);

        virtual void SetProjection(float left, float right, float bottom, float top);
        virtual void SetProjection(float aspectRatio, float zoomLevel);

        virtual void SetPosition(const glm::vec3 &position) override;
        virtual void SetRotationDegrees(const glm::vec3 &rotation) override; // In degrees

        float GetAspectRatio() const { return m_AspectRatio; }
        float GetZoomLevel() const { return m_ZoomLevel; }
        float GetNearClip() const { return zNear; }
        float GetFarClip() const { return zFar; }

    private:
        void RecalculateViewMatrix();

    private:
        float m_AspectRatio;
        float m_ZoomLevel;
        const float zNear = -100.0f;
        const float zFar = 100.0f;
    };
}