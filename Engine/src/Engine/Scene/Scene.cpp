#include "Scene.h"
#include "Engine/Renderer/Renderer2D.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/NativeScriptComponent.h"
#include "Engine/Renderer/Camera/OrthographicCamera.h"
#include "Engine/Renderer/Camera/PerspectiveCamera.h"
#include "Engine/Profile/Instrumentor.h"

namespace Engine
{
    void Scene::OnUpdate(Core::Timestep deltaTime)
    {
        ENGINE_PROFILING_FUNC();
        // update scripts
        {
            m_Registry.view<NativeScriptComponent>().each([=](auto entity, NativeScriptComponent &nsc)
                                                          {
                if (!nsc.Instance)
                {
                    nsc.Instance = nsc.Instantiate();
                    nsc.Instance->m_Entity = Entity(entity, this);
                    nsc.Instance->OnCreate();
                }
                nsc.Instance->OnUpdate(deltaTime); });
        }

        // Update Cameras
        m_CameraEntity = nullptr;
        {
            auto view = m_Registry.view<TransformComponent, CameraComponent>();
            for (auto entity : view)
            {
                auto &cameraComponent = view.get<CameraComponent>(entity);
                auto &transformComponent = view.get<TransformComponent>(entity);

                if (cameraComponent.Primary)
                {
                    m_CameraEntity = CreateRef<Entity>(entity, this);
                }
            }
        }

        if (m_CameraEntity == nullptr)
        {
            LOG_WARN("No primary camera found in the scene!");
            return;
        }

        // Calaculate camera view projection matrix
        glm::mat4 mainCameraViewProjection = glm::mat4(1.0f);
        {
            auto &cameraComponent = m_CameraEntity->GetComponent<CameraComponent>();
            auto &transformComponent = m_CameraEntity->GetComponent<TransformComponent>();

            glm::mat4 transform = transformComponent.GetTransform();
            glm::mat4 viewMatrix = glm::inverse(transform);
            mainCameraViewProjection = cameraComponent.Camera->GetProjectionMatrix() * viewMatrix;
        }

        Renderer::Renderer2D::BeginScene(mainCameraViewProjection);
        Renderer::Renderer2D::ResetStats();
        auto view = m_Registry.view<SpriteComponent, TransformComponent>();
        for (auto entity : view)
        {
            auto &spriteComponent = view.get<SpriteComponent>(entity);
            auto &transformComponent = view.get<TransformComponent>(entity);
            Renderer::Renderer2D::DrawQuad(transformComponent.GetTransform(), spriteComponent.Color);
        }

        Engine::Renderer::Renderer2D::EndScene();
    }

    Entity Scene::CreateEntity(const std::string &name)
    {
        entt::entity entityHandle = m_Registry.create();
        auto entity = Entity(entityHandle, this);
        entity.AddComponent<TagComponent>(name);
        return entity;
    }
    glm::vec2 Scene::ScreenToWorld(const glm::vec2 &screenPos)
    {
        if (m_CameraEntity == nullptr)
        {
            LOG_WARN("No primary camera found in the scene!");
            return glm::vec2(0.0f);
        }

        auto &cameraComponent = m_CameraEntity->GetComponent<CameraComponent>();
        auto &transformComponent = m_CameraEntity->GetComponent<TransformComponent>();

        glm::mat4 transform = transformComponent.GetTransform();
        glm::mat4 viewMatrix = glm::inverse(transform);
        glm::mat4 viewProjection = cameraComponent.Camera->GetProjectionMatrix() * viewMatrix;
        glm::mat4 inverseViewProjection = glm::inverse(viewProjection);

        // Normalized Device Coordinates
        float x = (2.0f * screenPos.x) / m_ViewportWidth - 1.0f;
        float y = 1.0f - (2.0f * screenPos.y) / m_ViewportHeight;
        glm::vec4 ndcPos = glm::vec4(x, y, 1.0f, 1.0f);

        glm::vec4 worldPos = inverseViewProjection * ndcPos;
        worldPos /= worldPos.w;

        return glm::vec2(worldPos);
    }
    void Scene::RecalculateCameraProjections()
    {
        auto view = m_Registry.view<CameraComponent>();
        float aspectRatio = (float)m_ViewportWidth / (float)m_ViewportHeight;
        for (auto entity : view)
        {
            auto &cameraComponent = view.get<CameraComponent>(entity);
            cameraComponent.Camera->SetAspectRatio(aspectRatio);
        }
    }
}
