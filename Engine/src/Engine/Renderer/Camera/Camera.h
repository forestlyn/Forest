#pragma once
#include <glm/glm.hpp>
namespace Engine::Renderer
{
    class Camera
    {
    public:
        virtual ~Camera() = default;

        virtual void SetPosition(const glm::vec3 &position) = 0;
        virtual void SetRotationDegrees(const glm::vec3 &rotation) = 0;
        virtual glm::vec3 GetPosition() const { return m_Position; }
        virtual glm::vec3 GetRotationDegrees() const { return m_RotationDegrees; }

        virtual const glm::mat4 &GetProjectionMatrix() const { return m_ProjectionMatrix; }
        virtual const glm::mat4 &GetViewMatrix() const { return m_ViewMatrix; }
        virtual const glm::mat4 &GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

    protected:
        glm::vec3 m_Position = glm::vec3(0.0f);
        glm::vec3 m_RotationDegrees = glm::vec3(0.0f);
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ViewProjectionMatrix;
    };
}