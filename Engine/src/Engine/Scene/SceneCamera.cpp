#include "SceneCamera.h"

namespace Engine
{
    SceneCamera::SceneCamera()
    {
        m_OrthographicCamera = CreateRef<Renderer::OrthographicCamera>(-m_AspectRatio * m_OrthographicSize, m_AspectRatio * m_OrthographicSize, -m_OrthographicSize, m_OrthographicSize);
        m_PerspectiveCamera = CreateRef<Renderer::PerspectiveCamera>(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);

        m_Camera = m_OrthographicCamera;
    }
    void SceneCamera::SetProjectionType(ProjectionType type)
    {
        m_ProjectionType = type;
        switch (m_ProjectionType)
        {
        case ProjectionType::Orthographic:
            m_Camera = m_OrthographicCamera;
            break;
        case ProjectionType::Perspective:
            m_Camera = m_PerspectiveCamera;
            break;
        }
    }

    void SceneCamera::SetAspectRatio(float aspectRatio)
    {
        m_AspectRatio = aspectRatio;
        m_OrthographicCamera->SetAspectRatio(m_AspectRatio);
        m_PerspectiveCamera->SetAspectRatio(m_AspectRatio);
    }
#pragma region PerspectiveCamera
    void SceneCamera::SetPerspectiveFOV(float fov)
    {
        m_PerspectiveFOV = fov;
        m_PerspectiveCamera->SetFOVDegrees(m_PerspectiveFOV);
    }
    void SceneCamera::SetPerspectiveNearClip(float nearClip)
    {
        m_PerspectiveNear = nearClip;
        m_PerspectiveCamera->SetNearClip(nearClip);
    }
    void SceneCamera::SetPerspectiveFarClip(float farClip)
    {
        m_PerspectiveFar = farClip;
        m_PerspectiveCamera->SetFarClip(farClip);
    }
#pragma endregion
#pragma region OrthographicCamera
    void SceneCamera::SetOrthographicSize(float size)
    {
        m_OrthographicSize = size;
        m_OrthographicCamera->SetZoomLevel(m_OrthographicSize);
    }
    void SceneCamera::SetOrthographicNearClip(float nearClip)
    {
        m_OrthographicNear = nearClip;
        m_OrthographicCamera->SetNearClip(nearClip);
    }
    void SceneCamera::SetOrthographicFarClip(float farClip)
    {
        m_OrthographicFar = farClip;
        m_OrthographicCamera->SetFarClip(farClip);
    }
#pragma endregion
} // namespace Engine