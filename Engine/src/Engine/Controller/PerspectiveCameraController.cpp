#include "PerspectiveCameraController.h"
#include "Engine/Core/MouseCode.h"
#include "Engine/Core/KeyCode.h"
#include "Engine/Events/Event.h"
#include "Engine/Core/Input.h"
#include "Engine/Profile/Instrumentor.h"
namespace Engine
{
    PerspectiveCameraController::PerspectiveCameraController(Entity *camera, glm::bvec3 rotation)
        : m_CameraEntity(camera), m_Rotation(rotation),
          m_CameraTransform(&m_CameraEntity->GetComponent<TransformComponent>()),
          m_CameraComponent(&m_CameraEntity->GetComponent<CameraComponent>())
    {
        SetCameraEntity(camera);
    }

    void PerspectiveCameraController::SetCameraEntity(Entity *camera)
    {
        m_CameraEntity = camera;
        m_CameraTransform = &m_CameraEntity->GetComponent<TransformComponent>();
        m_CameraComponent = &m_CameraEntity->GetComponent<CameraComponent>();
        m_PerspectiveCamera = static_cast<Renderer::PerspectiveCamera *>(m_CameraComponent->Camera.get());

        m_CameraPosition = m_CameraTransform->GetPosition();
        m_CameraRotation = m_CameraTransform->GetPosition();
    }

    void PerspectiveCameraController::OnUpdate(Core::Timestep timestep)
    {
        ENGINE_PROFILING_FUNC();
        m_CameraMoveSpeed = 1.0f;
        if (Core::Input::IsKeyPressed(FOREST_KEY_W))
            m_CameraPosition.z -= m_CameraMoveSpeed * timestep;
        if (Core::Input::IsKeyPressed(FOREST_KEY_S))
            m_CameraPosition.z += m_CameraMoveSpeed * timestep;
        if (Core::Input::IsKeyPressed(FOREST_KEY_A))
            m_CameraPosition.x -= m_CameraMoveSpeed * timestep;
        if (Core::Input::IsKeyPressed(FOREST_KEY_D))
            m_CameraPosition.x += m_CameraMoveSpeed * timestep;

        m_CameraTransform->SetPosition(m_CameraPosition);

        if (m_Rotation.x)
        {
            if (Core::Input::IsKeyPressed(FOREST_KEY_Q))
                m_CameraRotation.x -= m_CameraRotationSpeed * timestep;
            if (Core::Input::IsKeyPressed(FOREST_KEY_E))
                m_CameraRotation.x += m_CameraRotationSpeed * timestep;
            m_CameraTransform->SetRotation(m_CameraRotation);
        }

        if (m_Rotation.y)
        {
            if (Core::Input::IsKeyPressed(FOREST_KEY_Z))
                m_CameraRotation.y -= m_CameraRotationSpeed * timestep;
            if (Core::Input::IsKeyPressed(FOREST_KEY_C))
                m_CameraRotation.y += m_CameraRotationSpeed * timestep;
            m_CameraTransform->SetRotation(m_CameraRotation);
        }

        if (m_Rotation.z)
        {
            if (Core::Input::IsKeyPressed(FOREST_KEY_T))
                m_CameraRotation.z -= m_CameraRotationSpeed * timestep;
            if (Core::Input::IsKeyPressed(FOREST_KEY_Y))
                m_CameraRotation.z += m_CameraRotationSpeed * timestep;
            m_CameraTransform->SetRotation(m_CameraRotation);
        }
    }

    void PerspectiveCameraController::OnEvent(Event::Event &event)
    {
        ENGINE_PROFILING_FUNC();
        Event::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<Event::WindowResizeEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnWindowResize));
        dispatcher.Dispatch<Event::MouseScrolledEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnScroll));
    }

    bool PerspectiveCameraController::OnWindowResize(Event::WindowResizeEvent &event)
    {
        ENGINE_PROFILING_FUNC();
        if (event.GetHeight() == 0 || event.GetWidth() == 0)
            return false;
        float m_AspectRatio = (float)event.GetWidth() / (float)event.GetHeight();
        m_PerspectiveCamera->SetAspectRatio(m_AspectRatio);
        return false;
    }

    bool PerspectiveCameraController::OnScroll(Event::MouseScrolledEvent &event)
    {
        ENGINE_PROFILING_FUNC();
        float m_FOV = m_PerspectiveCamera->GetFOVDegrees();
        m_FOV -= event.GetYOffset();
        m_FOV = std::clamp(m_FOV, 1.0f, 90.0f);
        // ENGINE_INFO("FOV changed to: {}", m_FOV);
        m_PerspectiveCamera->SetFOVDegrees(m_FOV);
        return false;
    }

    glm::vec2 PerspectiveCameraController::ScreenToWorld(const glm::vec2 &screenPos, const glm::vec2 &viewportSize)
    {
        // 1. 将屏幕坐标 (0 到 width, 0 到 height) 转换为 NDC 坐标 (-1 到 1)
        // 注意：OpenGL 中 Y 轴通常是向上的，而屏幕坐标 Y 轴通常是向下的，所以需要反转 Y
        float ndcX = (screenPos.x / viewportSize.x) * 2.0f - 1.0f;
        float ndcY = -((screenPos.y / viewportSize.y) * 2.0f - 1.0f); // 反转 Y

        glm::vec4 ndcPos(ndcX, ndcY, 0.0f, 1.0f);

        // 2. 计算 ViewProjection 矩阵
        // View Matrix 是相机 Transform 的逆矩阵
        glm::mat4 transform = m_CameraTransform->GetTransform();

        glm::mat4 viewMatrix = glm::inverse(transform);

        // Projection Matrix 来自 Camera 对象
        glm::mat4 projMatrix = m_PerspectiveCamera->GetProjectionMatrix();

        glm::mat4 viewProj = projMatrix * viewMatrix;
        glm::mat4 invViewProj = glm::inverse(viewProj);

        // 3. 计算世界坐标
        glm::vec4 worldPos = invViewProj * ndcPos;

        // 对于正交相机，通常不需要除以 w，但在透视投影中是必须的。为了通用性可以加上。
        worldPos /= worldPos.w;

        return glm::vec2(worldPos.x, worldPos.y);
    }

    glm::mat4 PerspectiveCameraController::GetViewProjectionMatrix()
    {
        glm::mat4 transform = m_CameraTransform->GetTransform();
        glm::mat4 viewMatrix = glm::inverse(transform);
        glm::mat4 cameraViewProjection = m_PerspectiveCamera->GetProjectionMatrix() * viewMatrix;
        return cameraViewProjection;
    }
}