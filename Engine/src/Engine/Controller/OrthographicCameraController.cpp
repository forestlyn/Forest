#include "OrthographicCameraController.h"
#include "Engine/Core/MouseCode.h"
#include "Engine/Core/KeyCode.h"
#include "Engine/Events/Event.h"
#include "Engine/Core/Input.h"
#include "Engine/Profile/Instrumentor.h"
namespace Engine
{
    OrthographicCameraController::OrthographicCameraController(float aspectRatio, float zoomLevel, bool rotation)
        : m_AspectRatio(aspectRatio),
          m_ZoomLevel(zoomLevel),
          m_Camera(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel),
          m_Rotation(rotation)
    {
    }

    void OrthographicCameraController::OnUpdate(Core::Timestep timestep)
    {
        ENGINE_PROFILING_FUNC();
        m_CameraMoveSpeed = m_ZoomLevel;
        if (Core::Input::IsKeyPressed(FOREST_KEY_W))
            m_CameraPosition.y -= m_CameraMoveSpeed * timestep;
        if (Core::Input::IsKeyPressed(FOREST_KEY_S))
            m_CameraPosition.y += m_CameraMoveSpeed * timestep;
        if (Core::Input::IsKeyPressed(FOREST_KEY_A))
            m_CameraPosition.x -= m_CameraMoveSpeed * timestep;
        if (Core::Input::IsKeyPressed(FOREST_KEY_D))
            m_CameraPosition.x += m_CameraMoveSpeed * timestep;

        m_Camera.SetPosition(m_CameraPosition);

        if (m_Rotation)
        {
            if (Core::Input::IsKeyPressed(FOREST_KEY_Q))
                m_CameraRotation.z -= m_CameraRotationSpeed * timestep;
            if (Core::Input::IsKeyPressed(FOREST_KEY_E))
                m_CameraRotation.z += m_CameraRotationSpeed * timestep;
            m_Camera.SetRotationDegrees(m_CameraRotation);
        }
    }
    void OrthographicCameraController::OnEvent(Event::Event &event)
    {
        ENGINE_PROFILING_FUNC();
        Event::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<Event::WindowResizeEvent>(BIND_EVENT_FN(OrthographicCameraController::OnWindowResize));
        dispatcher.Dispatch<Event::MouseScrolledEvent>(BIND_EVENT_FN(OrthographicCameraController::OnScroll));
    }
    void OrthographicCameraController::OnResize(float width, float height)
    {
        ENGINE_PROFILING_FUNC();
        if (height == 0.0f)
            return;
        m_AspectRatio = width / height;
        ENGINE_INFO("Camera Resized: New Aspect Ratio: {}", m_AspectRatio);
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
    }
    bool OrthographicCameraController::OnWindowResize(Event::WindowResizeEvent &event)
    {
        ENGINE_PROFILING_FUNC();
        OnResize((float)event.GetWidth(), (float)event.GetHeight());
        return false;
    }
    bool OrthographicCameraController::OnScroll(Event::MouseScrolledEvent &event)
    {
        ENGINE_PROFILING_FUNC();
        m_ZoomLevel -= event.GetYOffset() * 0.1f;
        m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
        return false;
    }
}