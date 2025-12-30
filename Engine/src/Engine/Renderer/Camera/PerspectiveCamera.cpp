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
        RecalculateProjectMatrix();
    }

    void PerspectiveCamera::RecalculateProjectMatrix()
    {
        m_ProjectionMatrix = glm::perspectiveRH_NO(glm::radians(m_FOVDegrees), m_AspectRatio, m_NearClip, m_FarClip);
    }
} // namespace Engine::Renderer