#include "PerspectiveCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include "Engine/pcheader.h"
#include "Engine/Profile/Instrumentor.h"
namespace Engine::Renderer
{
    PerspectiveCamera::PerspectiveCamera(float fovDegrees, float aspectRatio, float nearClip, float farClip)
        : m_FOVDegrees(fovDegrees), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip)
    {
        ENGINE_INFO("Creating PerspectiveCamera with FOV: {}, Aspect Ratio: {}, Near Clip: {}, Far Clip: {}", fovDegrees, aspectRatio, nearClip, farClip);
        m_ProjectionMatrix = glm::perspectiveRH_NO(glm::radians(fovDegrees), aspectRatio, nearClip, farClip);
        m_ViewMatrix = glm::mat4(1.0f);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        m_InverseViewProjectionMatrix = glm::inverse(m_ViewProjectionMatrix);
    }

    void PerspectiveCamera::SetProjection(float fovDegrees, float aspectRatio, float nearClip, float farClip)
    {
        ENGINE_PROFILING_FUNC();
        m_FOVDegrees = fovDegrees;
        m_AspectRatio = aspectRatio;
        m_NearClip = nearClip;
        m_FarClip = farClip;
        m_ProjectionMatrix = glm::perspectiveRH_NO(glm::radians(fovDegrees), aspectRatio, nearClip, farClip);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        m_InverseViewProjectionMatrix = glm::inverse(m_ViewProjectionMatrix);
    }

    void PerspectiveCamera::SetPosition(const glm::vec3 &position)
    {
        ENGINE_PROFILING_FUNC();

        m_Position = position;
        RecalculateViewMatrix();
    }

    void PerspectiveCamera::SetRotationDegrees(const glm::vec3 &rotation)
    {
        ENGINE_PROFILING_FUNC();

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
        m_InverseViewProjectionMatrix = glm::inverse(m_ViewProjectionMatrix);
    }
} // namespace Engine::Renderer