#include "Scene.h"
#include "Engine/Renderer/Renderer2D.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/NativeScriptComponent.h"
#include "Engine/Renderer/Camera/OrthographicCamera.h"
#include "Engine/Renderer/Camera/PerspectiveCamera.h"
namespace Engine
{
    void Scene::OnUpdate(Core::Timestep deltaTime)
    {

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
        entt::entity mainCameraEntity = entt::null;
        {
            auto view = m_Registry.view<TransformComponent, CameraComponent>();
            for (auto entity : view)
            {
                auto &cameraComponent = view.get<CameraComponent>(entity);
                auto &transformComponent = view.get<TransformComponent>(entity);

                if (cameraComponent.Primary)
                {
                    mainCameraEntity = entity;
                }
            }
        }

        if (mainCameraEntity == entt::null)
        {
            LOG_WARN("No primary camera found in the scene!");
            return;
        }

        // Calaculate camera view projection matrix
        glm::mat4 mainCameraViewProjection = glm::mat4(1.0f);
        {
            auto &cameraComponent = m_Registry.get<CameraComponent>(mainCameraEntity);
            auto &transformComponent = m_Registry.get<TransformComponent>(mainCameraEntity);

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
    void Scene::RecalculateCameraProjections()
    {
        auto view = m_Registry.view<CameraComponent>();
        float aspectRatio = (float)m_ViewportWidth / (float)m_ViewportHeight;
        for (auto entity : view)
        {
            auto &cameraComponent = view.get<CameraComponent>(entity);
            if (auto orthographicCamera = dynamic_cast<Renderer::OrthographicCamera *>(cameraComponent.Camera.get()))
            {
                orthographicCamera->SetAspectRatio(aspectRatio);
            }
            else if (auto perspectiveCamera = dynamic_cast<Renderer::PerspectiveCamera *>(cameraComponent.Camera.get()))
            {
                perspectiveCamera->SetAspectRatio(aspectRatio);
            }
        }
    }
}
