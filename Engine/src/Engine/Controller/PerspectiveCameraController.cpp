#include "PerspectiveCameraController.h"
#include "Engine/Core/MouseCode.h"
#include "Engine/Core/KeyCode.h"
#include "Engine/Events/Event.h"
#include "Engine/Core/Input.h"

namespace Engine
{
    PerspectiveCameraController::PerspectiveCameraController(float fov, float aspectRatio, glm::bvec3 rotation)
        : m_AspectRatio(aspectRatio),
          m_FOV(fov),
          m_Camera(fov, aspectRatio, 0.1f, 1000.0f),
          m_Rotation(rotation)
    {
    }

    void PerspectiveCameraController::OnUpdate(Core::Timestep timestep)
    {
        m_CameraMoveSpeed = 1.0f;
        if (Core::Input::IsKeyPressed(FOREST_KEY_W))
            m_CameraPosition.z -= m_CameraMoveSpeed * timestep;
        if (Core::Input::IsKeyPressed(FOREST_KEY_S))
            m_CameraPosition.z += m_CameraMoveSpeed * timestep;
        if (Core::Input::IsKeyPressed(FOREST_KEY_A))
            m_CameraPosition.x -= m_CameraMoveSpeed * timestep;
        if (Core::Input::IsKeyPressed(FOREST_KEY_D))
            m_CameraPosition.x += m_CameraMoveSpeed * timestep;

        m_Camera.SetPosition(m_CameraPosition);

        if (m_Rotation.x)
        {
            if (Core::Input::IsKeyPressed(FOREST_KEY_Q))
                m_CameraRotation.x -= m_CameraRotationSpeed * timestep;
            if (Core::Input::IsKeyPressed(FOREST_KEY_E))
                m_CameraRotation.x += m_CameraRotationSpeed * timestep;
            m_Camera.SetRotationDegrees(m_CameraRotation);
        }

        if (m_Rotation.y)
        {
            if (Core::Input::IsKeyPressed(FOREST_KEY_Z))
                m_CameraRotation.y -= m_CameraRotationSpeed * timestep;
            if (Core::Input::IsKeyPressed(FOREST_KEY_C))
                m_CameraRotation.y += m_CameraRotationSpeed * timestep;
            m_Camera.SetRotationDegrees(m_CameraRotation);
        }

        if (m_Rotation.z)
        {
            if (Core::Input::IsKeyPressed(FOREST_KEY_T))
                m_CameraRotation.z -= m_CameraRotationSpeed * timestep;
            if (Core::Input::IsKeyPressed(FOREST_KEY_Y))
                m_CameraRotation.z += m_CameraRotationSpeed * timestep;
            m_Camera.SetRotationDegrees(m_CameraRotation);
        }
    }

    void PerspectiveCameraController::OnEvent(Event::Event &event)
    {
        Event::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<Event::WindowResizeEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnWindowResize));
        dispatcher.Dispatch<Event::MouseScrolledEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnScroll));
    }

    bool PerspectiveCameraController::OnWindowResize(Event::WindowResizeEvent &event)
    {
        m_AspectRatio = (float)event.GetWidth() / (float)event.GetHeight();
        m_Camera.SetProjection(m_FOV, m_AspectRatio, 0.1f, 1000.0f);
        return false;
    }

    bool PerspectiveCameraController::OnScroll(Event::MouseScrolledEvent &event)
    {
        m_FOV -= event.GetYOffset();
        m_FOV = std::clamp(m_FOV, 1.0f, 90.0f);
        // ENGINE_INFO("FOV changed to: {}", m_FOV);
        m_Camera.SetProjection(m_FOV, m_AspectRatio, 0.1f, 1000.0f);
        return false;
    }
}