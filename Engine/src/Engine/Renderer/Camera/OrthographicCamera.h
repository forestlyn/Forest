#pragma once
#include "Camera.h"
namespace Engine::Renderer
{
    class OrthographicCamera : public Camera
    {
    public:
        OrthographicCamera(float left, float right, float bottom, float top);

        virtual void SetProjection(float left, float right, float bottom, float top);

        virtual void SetPosition(const glm::vec3 &position) override;
        virtual void SetRotationDegrees(const glm::vec3 &rotation) override; // In degrees
    private:
        void RecalculateViewMatrix();
    };
}