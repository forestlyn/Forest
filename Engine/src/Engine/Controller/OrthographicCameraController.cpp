#include "OrthographicCameraController.h"
#include "Engine/Core/MouseCode.h"
#include "Engine/Core/KeyCode.h"
#include "Engine/Events/Event.h"
#include "Engine/Core/Input.h"
#include "Engine/Profile/Instrumentor.h"
namespace Engine
{

    OrthographicCameraController::OrthographicCameraController(Entity *camera, bool rotation)
        : m_CameraEntity(camera), m_Rotation(rotation),
          m_CameraTransform(&camera->GetComponent<TransformComponent>()),
          m_CameraComponent(&camera->GetComponent<CameraComponent>())
    {
        SetCameraEntity(camera);
    }

    void OrthographicCameraController::SetCameraEntity(Entity *camera)
    {
        m_CameraEntity = camera;
        m_CameraTransform = &m_CameraEntity->GetComponent<TransformComponent>();
        m_CameraComponent = &m_CameraEntity->GetComponent<CameraComponent>();
        m_OrthoCamera = static_cast<Renderer::OrthographicCamera *>(m_CameraComponent->Camera.get());

        m_CameraPosition = m_CameraTransform->GetPosition();
        m_CameraRotation = m_CameraTransform->GetRotation();
    }

    void OrthographicCameraController::OnUpdate(Core::Timestep timestep)
    {

        ENGINE_PROFILING_FUNC();
        if (Core::Input::IsKeyPressed(FOREST_KEY_W))
            m_CameraPosition.y -= m_CameraMoveSpeed * timestep;
        if (Core::Input::IsKeyPressed(FOREST_KEY_S))
            m_CameraPosition.y += m_CameraMoveSpeed * timestep;
        if (Core::Input::IsKeyPressed(FOREST_KEY_A))
            m_CameraPosition.x -= m_CameraMoveSpeed * timestep;
        if (Core::Input::IsKeyPressed(FOREST_KEY_D))
            m_CameraPosition.x += m_CameraMoveSpeed * timestep;

        m_CameraTransform->SetPosition(m_CameraPosition);

        if (m_Rotation)
        {
            if (Core::Input::IsKeyPressed(FOREST_KEY_Q))
                m_CameraRotation.z -= m_CameraRotationSpeed * timestep;
            if (Core::Input::IsKeyPressed(FOREST_KEY_E))
                m_CameraRotation.z += m_CameraRotationSpeed * timestep;
            m_CameraTransform->SetRotation(m_CameraRotation);
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
        float m_AspectRatio = width / height;
        m_OrthoCamera->SetAspectRatio(m_AspectRatio);
    }

    glm::vec2 OrthographicCameraController::ScreenToWorld(const glm::vec2 &screenPos, const glm::vec2 &viewportSize)
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
        glm::mat4 projMatrix = m_OrthoCamera->GetProjectionMatrix();

        glm::mat4 viewProj = projMatrix * viewMatrix;
        glm::mat4 invViewProj = glm::inverse(viewProj);

        // 3. 计算世界坐标
        glm::vec4 worldPos = invViewProj * ndcPos;

        // 对于正交相机，通常不需要除以 w，但在透视投影中是必须的。为了通用性可以加上。
        // worldPos /= worldPos.w;

        return glm::vec2(worldPos.x, worldPos.y);
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
        float m_ZoomLevel = m_OrthoCamera->GetZoomLevel();
        m_ZoomLevel -= event.GetYOffset() * 0.1f;
        m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
        m_OrthoCamera->SetZoomLevel(m_ZoomLevel);
        return false;
    }

    glm::mat4 OrthographicCameraController::GetViewProjectionMatrix()
    {
        glm::mat4 transform = m_CameraTransform->GetTransform();
        glm::mat4 viewMatrix = glm::inverse(transform);
        glm::mat4 cameraViewProjection = m_OrthoCamera->GetProjectionMatrix() * viewMatrix;
        return cameraViewProjection;
    }
}