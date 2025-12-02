#include "PerspectiveCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include "Engine/pcheader.h"
namespace Engine::Renderer
{
    PerspectiveCamera::PerspectiveCamera(float fovDegrees, float aspectRatio, float nearClip, float farClip)
    {
        // ENGINE_INFO("Creating PerspectiveCamera with FOV: {}, Aspect Ratio: {}, Near Clip: {}, Far Clip: {}", fovDegrees, aspectRatio, nearClip, farClip);
        m_ProjectionMatrix = glm::perspectiveRH_NO(glm::radians(fovDegrees), aspectRatio, nearClip, farClip);
        m_ViewMatrix = glm::mat4(1.0f);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        // ENGINE_TRACE("ProjectionMatrix:");
        // ENGINE_TRACE("{0}", glm::to_string(m_ProjectionMatrix));

        // ENGINE_TRACE("ViewMatrix:");
        // ENGINE_TRACE("{0}", glm::to_string(m_ViewMatrix));

        // ENGINE_TRACE("ViewProjectionMatrix:");
        // ENGINE_TRACE("{0}", glm::to_string(m_ViewProjectionMatrix));
    }

    void PerspectiveCamera::SetPosition(const glm::vec3 &position)
    {
        m_Position = position;
        RecalculateViewMatrix();
    }

    void PerspectiveCamera::SetRotationDegrees(const glm::vec3 &rotation)
    {
        m_RotationDegrees = rotation;
        RecalculateViewMatrix();
    }

    void PerspectiveCamera::RecalculateViewMatrix()
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(m_RotationDegrees.z), glm::vec3(0, 0, 1)) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(m_RotationDegrees.y), glm::vec3(0, 1, 0)) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(m_RotationDegrees.x), glm::vec3(1, 0, 0));

        m_ViewMatrix = glm::inverse(transform);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        // ENGINE_TRACE("=== Camera Debug Info ===");
        // ENGINE_TRACE("Camera Position: ({}, {}, {})", m_Position.x, m_Position.y, m_Position.z);
        // ENGINE_TRACE("Camera Rotation: ({}, {}, {}) degrees", m_RotationDegrees.x, m_RotationDegrees.y, m_RotationDegrees.z);

        // ENGINE_TRACE("ProjectionMatrix:");
        // ENGINE_TRACE("{0}", glm::to_string(m_ProjectionMatrix));

        // ENGINE_TRACE("ViewMatrix:");
        // ENGINE_TRACE("{0}", glm::to_string(m_ViewMatrix));

        // ENGINE_TRACE("ViewProjectionMatrix:");
        // ENGINE_TRACE("{0}", glm::to_string(m_ViewProjectionMatrix));
        // ENGINE_TRACE("=== End Camera Debug ===");
    }
} // namespace Engine::Renderer