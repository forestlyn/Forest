#include "Scene.h"
#include "Engine/Renderer/Renderer2D.h"
#include "Engine/Scene/Entity.h"

namespace Engine
{
    void Scene::OnUpdate(Core::Timestep deltaTime)
    {
        // Update Cameras
        Renderer::Camera *mainCamera = nullptr;
        {
            auto view = m_Registry.view<TransformComponent, CameraComponent>();
            for (auto entity : view)
            {
                auto &cameraComponent = view.get<CameraComponent>(entity);
                auto &transformComponent = view.get<TransformComponent>(entity);

                cameraComponent.Camera->SetPosition(transformComponent.Position);
                cameraComponent.Camera->SetRotationDegrees(transformComponent.Rotation);

                if (cameraComponent.Primary)
                {
                    mainCamera = cameraComponent.Camera.get();
                }
            }
        }

        if (!mainCamera)
        {
            LOG_WARN("No primary camera found in the scene!");
            return;
        }

        Renderer::Renderer2D::BeginScene(*mainCamera);
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
}
