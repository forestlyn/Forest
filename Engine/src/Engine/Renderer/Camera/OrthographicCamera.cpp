#include "OrthographicCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Engine/pcheader.h"
#include "Engine/Profile/Instrumentor.h"
namespace Engine::Renderer
{
    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
    {
        m_AspectRatio = (right - left) / (top - bottom);
        m_ZoomLevel = (top - bottom) / 2.0f;
        RecalculateProjectMatrix();
    }

    OrthographicCamera::OrthographicCamera(float aspectRatio, float zoomLevel)
        : m_AspectRatio(aspectRatio), m_ZoomLevel(zoomLevel)
    {
        RecalculateProjectMatrix();
    }

    void OrthographicCamera::RecalculateProjectMatrix()
    {
        ENGINE_PROFILING_FUNC();
        m_ProjectionMatrix = glm::ortho(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel, m_NearClip, m_FarClip);
    }

} // namespace Engine::Renderer