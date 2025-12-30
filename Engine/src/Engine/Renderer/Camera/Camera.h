#pragma once
#include <glm/glm.hpp>
#include "Engine/Core/Application.h"

// only concerned with projection matrix here
namespace Engine::Renderer
{
    class Camera
    {
    public:
        virtual ~Camera() = default;

        virtual const glm::mat4 &GetProjectionMatrix() const { return m_ProjectionMatrix; }

    protected:
        glm::mat4 m_ProjectionMatrix;
    };
}