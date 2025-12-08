#include "OrthographicCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Engine/pcheader.h"
namespace Engine::Renderer
{
    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
    {
        m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
        m_ViewMatrix = glm::mat4(1.0f);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
    {
        m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void OrthographicCamera::SetPosition(const glm::vec3 &position)
    {
        m_Position = position;
        RecalculateViewMatrix();
    }

    void OrthographicCamera::SetRotationDegrees(const glm::vec3 &rotation)
    {
        m_RotationDegrees = rotation;
        if (m_RotationDegrees.x != 0.0f || m_RotationDegrees.y != 0.0f)
        {
            ENGINE_WARN("OrthographicCamera only supports rotation around Z axis. X and Y rotations will be ignored.");
        }
        RecalculateViewMatrix();
    }

    void OrthographicCamera::RecalculateViewMatrix()
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(m_RotationDegrees.z), glm::vec3(0, 0, 1));

        m_ViewMatrix = glm::inverse(transform);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }
} // namespace Engine::Renderer