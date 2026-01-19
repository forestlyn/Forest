#include "Scene.h"
#include "Engine/Renderer/Renderer2D.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/NativeScriptComponent.h"
#include "Engine/Renderer/Camera/OrthographicCamera.h"
#include "Engine/Renderer/Camera/PerspectiveCamera.h"
#include "Engine/Profile/Instrumentor.h"
namespace Engine
{
    void Scene::OnUpdateRuntime(Core::Timestep deltaTime)
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

        // Remove entities marked for deletion
        {
            auto view = m_Registry.view<TagComponent>();
            std::vector<entt::entity> entitiesToRemove;
            for (auto entity : view)
            {
                if (view.get<TagComponent>(entity).IsRemove())
                {
                    entitiesToRemove.push_back(entity);
                }
            }

            for (auto entity : entitiesToRemove)
            {
                Entity e{entity, this};
                ENGINE_INFO("Really Removing entity {}", e.GetComponent<TagComponent>().Tag);
                if (e.HasComponent<NativeScriptComponent>())
                {
                    auto &nsc = e.GetComponent<NativeScriptComponent>();
                    if (nsc.Instance)
                    {
                        nsc.Instance->OnDestroy();
                        nsc.Destroy(&nsc);
                    }
                }
                m_Registry.destroy(entity);
            }
        }

        // Physics2D update
        {
            const float physicsStep = 1.0f / 60.0f;
            physicsTimeStepAccumulator += deltaTime.GetSeconds();
            if (physicsTimeStepAccumulator >= physicsStep)
            {
                physicsTimeStepAccumulator -= physicsStep;
                int subStepCount = 4;
                b2World_Step(worldId, physicsStep, subStepCount);

                // update transforms from physics
                {
                    auto view = m_Registry.view<Rigidbody2DComponent, TransformComponent>();
                    for (auto entity : view)
                    {
                        auto &rigidbody2D = view.get<Rigidbody2DComponent>(entity);
                        auto &transform = view.get<TransformComponent>(entity);

                        b2Vec2 position = b2Body_GetPosition(rigidbody2D.RuntimeBodyId);
                        float angle = b2Rot_GetAngle(b2Body_GetRotation(rigidbody2D.RuntimeBodyId));

                        transform.SetPosition(glm::vec3(position.x, position.y, transform.GetPosition().z));
                        transform.SetRotation(glm::vec3(transform.GetRotation().x, transform.GetRotation().y, glm::degrees(angle)));
                    }
                }
            }
        }

        // Render 2D
        {

            // Update Cameras
            bool foundPrimary = false;
            {
                auto view = m_Registry.view<TransformComponent, CameraComponent>();
                for (auto entity : view)
                {
                    auto &cameraComponent = view.get<CameraComponent>(entity);
                    auto &transformComponent = view.get<TransformComponent>(entity);
                    if (cameraComponent.Primary)
                    {
                        foundPrimary = true;
                        m_CameraEntity = CreateRef<Entity>(entity, this);
                        break;
                    }
                }
            }

            if (!foundPrimary)
            {
                m_CameraEntity = nullptr;
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

            auto circleView = m_Registry.view<CircleComponent, TransformComponent>();
            for (auto entity : circleView)
            {
                auto &circleComponent = circleView.get<CircleComponent>(entity);
                auto &transformComponent = circleView.get<TransformComponent>(entity);
                Renderer::Renderer2D::DrawCircle(transformComponent.GetTransform(), circleComponent, (int)entity);
            }

            Engine::Renderer::Renderer2D::EndScene();
        }
    }

    void Scene::OnUpdateEditor(Engine::Core::Timestep timestep, Renderer::EditorCamera &editorCamera)
    {
        ENGINE_PROFILING_FUNC();
        // Update Editor Camera

        glm::mat4 editorCameraViewProjection = editorCamera.GetViewProjectionMatrix();

        Renderer::Renderer2D::BeginScene(editorCameraViewProjection);
        Renderer::Renderer2D::ResetStats();
        auto view = m_Registry.view<SpriteComponent, TransformComponent>();
        for (auto entity : view)
        {
            auto &spriteComponent = view.get<SpriteComponent>(entity);
            auto &transformComponent = view.get<TransformComponent>(entity);
            Renderer::Renderer2D::DrawSprite(transformComponent.GetTransform(), spriteComponent, (int)entity);
        }

        auto circleView = m_Registry.view<CircleComponent, TransformComponent>();
        for (auto entity : circleView)
        {
            auto &circleComponent = circleView.get<CircleComponent>(entity);
            auto &transformComponent = circleView.get<TransformComponent>(entity);
            Renderer::Renderer2D::DrawCircle(transformComponent.GetTransform(), circleComponent, (int)entity);
        }

        Engine::Renderer::Renderer2D::EndScene();
    }

    Entity Scene::CreateEntity(const std::string &name)
    {
        entt::entity entityHandle = m_Registry.create();
        auto entity = Entity(entityHandle, this);
        entity.AddComponent<IDComponent>();
        entity.AddComponent<TagComponent>(name);
        entity.AddComponent<TransformComponent>();
        return entity;
    }

    Entity Scene::CreateEntityWithID(UUID uuid)
    {
        entt::entity entityHandle = m_Registry.create();
        Entity entity = Entity(entityHandle, this);
        entity.AddComponent<IDComponent>(uuid);
        return entity;
    }

    void Scene::SetViewportSize(uint32_t width, uint32_t height)
    {
        m_ViewportWidth = width;
        m_ViewportHeight = height;
        RecalculateCameraProjections();
    }

    Entity Scene::GetPrimaryCameraEntity()
    {
        return m_CameraEntity ? *m_CameraEntity : Entity{};
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
    void Scene::OnRuntimeStart()
    {
        ENGINE_INFO("Starting physics world");
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = b2Vec2(0.0f, -9.8f);
        worldId = b2CreateWorld(&worldDef);

        auto view = m_Registry.view<Rigidbody2DComponent, TransformComponent>();
        for (auto entityId : view)
        {
            ENGINE_INFO("Creating physics body for entity {}", (int)entityId);
            Entity entity = {entityId, this};
            auto &rigidbody2D = view.get<Rigidbody2DComponent>(entity);
            auto &transform = view.get<TransformComponent>(entity);

            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = (b2BodyType)rigidbody2D.Type;
            bodyDef.position = b2Vec2(transform.GetPosition().x, transform.GetPosition().y);
            bodyDef.rotation = b2MakeRot(glm::radians(transform.GetRotation().z));
            rigidbody2D.RuntimeBodyId = b2CreateBody(worldId, &bodyDef);

            if (entity.HasComponent<BoxCollider2DComponent>())
            {
                auto &boxCollider = entity.GetComponent<BoxCollider2DComponent>();
                b2Polygon dynamicBox = b2MakeBox(boxCollider.Size.x * transform.GetScale().x * 0.5f,
                                                 boxCollider.Size.y * transform.GetScale().y * 0.5f);
                b2ShapeDef shapeDef = b2DefaultShapeDef();
                shapeDef.density = boxCollider.Density;
                shapeDef.material.friction = boxCollider.Friction;
                shapeDef.material.restitution = boxCollider.Restitution;
                b2CreatePolygonShape(rigidbody2D.RuntimeBodyId, &shapeDef, &dynamicBox);
            }
        }
    }
    void Scene::OnRuntimeStop()
    {
        b2DestroyWorld(worldId);
    }

    template <typename T>
    static void CopyComponent(entt::registry &dst, entt::registry &src, std::unordered_map<uint64_t, entt::entity> entityMap)
    {
        auto view = src.view<T>();
        for (auto e : view)
        {
            ENGINE_ASSERT(entityMap.find(src.get<IDComponent>(e).ID) != entityMap.end(), "Entity not found in map!");
            entt::entity destEntity = entityMap[src.get<IDComponent>(e).ID];
            T &srcComponent = src.get<T>(e);
            dst.emplace_or_replace<T>(destEntity, srcComponent);
        }
    }

    Ref<Scene> Scene::Copy(Ref<Scene> other)
    {
        Ref<Scene> newScene = CreateRef<Scene>();
        newScene->m_ViewportWidth = other->m_ViewportWidth;
        newScene->m_ViewportHeight = other->m_ViewportHeight;

        std::unordered_map<uint64_t, entt::entity> entityMap;

        auto view = other->m_Registry.view<IDComponent>();
        for (auto e : view)
        {
            UUID uuid = other->m_Registry.get<IDComponent>(e).ID;
            std::string name = other->m_Registry.get<TagComponent>(e).Tag;
            Entity newEntity = newScene->CreateEntityWithID(uuid);
            entityMap[uuid] = (entt::entity)newEntity;
        }

        // Copy all components
        CopyComponent<TagComponent>(newScene->m_Registry, other->m_Registry, entityMap);
        CopyComponent<TransformComponent>(newScene->m_Registry, other->m_Registry, entityMap);
        CopyComponent<CameraComponent>(newScene->m_Registry, other->m_Registry, entityMap);
        CopyComponent<SpriteComponent>(newScene->m_Registry, other->m_Registry, entityMap);
        CopyComponent<CircleComponent>(newScene->m_Registry, other->m_Registry, entityMap);
        CopyComponent<Rigidbody2DComponent>(newScene->m_Registry, other->m_Registry, entityMap);
        CopyComponent<BoxCollider2DComponent>(newScene->m_Registry, other->m_Registry, entityMap);
        CopyComponent<NativeScriptComponent>(newScene->m_Registry, other->m_Registry, entityMap);

        return newScene;
    }

    template <typename T>
    static void CopyComponentIfExists(Entity dst, Entity src)
    {
        if (src.HasComponent<T>())
        {
            T &srcComponent = src.GetComponent<T>();
            dst.AddOrReplaceComponent<T>(srcComponent);
        }
    }

    void Scene::DuplicateEntity(Entity entity)
    {
        std::string name = entity.GetName();
        Entity newEntity = CreateEntity(name + "_Copy");
        CopyComponentIfExists<TransformComponent>(newEntity, entity);
        CopyComponentIfExists<CameraComponent>(newEntity, entity);
        CopyComponentIfExists<SpriteComponent>(newEntity, entity);
        CopyComponentIfExists<CircleComponent>(newEntity, entity);
        CopyComponentIfExists<Rigidbody2DComponent>(newEntity, entity);
        CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
        CopyComponentIfExists<NativeScriptComponent>(newEntity, entity);
    }

    void Scene::RecalculateCameraProjections()
    {
        auto view = m_Registry.view<CameraComponent>();
        float aspectRatio = (float)m_ViewportWidth / (float)m_ViewportHeight;
        for (auto entity : view)
        {
            auto &cameraComponent = view.get<CameraComponent>(entity);
            if (cameraComponent.FixedAspectRatio)
                continue;
            cameraComponent.Camera->SetAspectRatio(aspectRatio);
        }
    }
}
