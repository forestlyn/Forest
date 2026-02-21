#pragma once
#include "Engine/pcheader.h"
#include "Engine/Scene/Component.h"
#include "Engine/Core/Timestep.h"
#include <entt.hpp>
#include "Engine/Renderer/Camera/EditorCamera.h"
#include <box2d/box2d.h>
#include "Engine/Core/UUID.h"
namespace Engine
{
    class Entity;
    class Scene
    {
    public:
        Scene() = default;
        ~Scene()
        {
            LOG_INFO("Scene destroyed.");
        }
        void OnUpdateRuntime(Core::Timestep timestep);
        void OnUpdateEditor(Core::Timestep timestep, Renderer::EditorCamera &editorCamera);
        void OnUpdateSimulate(Core::Timestep timestep, Renderer::EditorCamera &editorCamera);

        // Create an entity with a specific name,will add TagComponent and TransformComponent by default
        Entity CreateEntity(const std::string &name = std::string());
        // Create an entity with a specific ID (for deserialization) no default components
        Entity CreateEntityWithID(UUID uuid);
        entt::registry &GetRegistry() { return m_Registry; }

        Entity GetEntityByUUID(UUID uuid);

        void SetViewportSize(uint32_t width, uint32_t height);

        Entity GetPrimaryCameraEntity();
        glm::mat4 GetPrimaryCameraViewProjectionMatrix();
        glm::vec2 ScreenToWorld(const glm::vec2 &screenPos);

        void OnRuntimeStart();
        void OnRuntimeStop();

        void OnSimulationStart();
        void OnSimulationStop();

        bool IsRunning() const
        {
            return m_Running;
        };

        static Ref<Scene> Copy(Ref<Scene> other);

        void DuplicateEntity(Entity entity);

    private:
        void RecalculateCameraProjections();

        void SetupPhysicsWorld();
        void DestroyPhysicsWorld();

        void StepPhysicsWorld(Core::Timestep timestep);
        void RenderScene2D(glm::mat4 viewProjectionMatrix);

    private:
        entt::registry m_Registry;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        Ref<Entity> m_CameraEntity = nullptr;
        b2WorldId worldId;

        bool m_Running = false;

        float physicsTimeStepAccumulator = 0.0f;

        std::unordered_map<UUID, entt::entity> m_EntityMap;

        friend class Entity;
        friend class EngineEditor::SceneHierarchyPanel;
    };

} // namespace Engine