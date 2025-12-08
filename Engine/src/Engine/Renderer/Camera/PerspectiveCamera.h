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
    private:
        void RecalculateViewMatrix();
    };
}