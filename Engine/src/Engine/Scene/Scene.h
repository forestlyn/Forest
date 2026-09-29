#pragma once
#include "Engine/pcheader.h"
#include "Engine/Scene/Component.h"
#include "Engine/Core/Timestep.h"
#include <entt.hpp>
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
        void OnUpdateRuntime(Core::Timestep timestep, bool drawUI = true);
        void OnUpdateEditor(Core::Timestep timestep, const glm::mat4 &viewProjectionMatrix, bool drawUI = true);
        void OnUpdateSimulate(Core::Timestep timestep, const glm::mat4 &viewProjectionMatrix, bool drawUI = true);
        // Editors can defer this until after their world-space debug overlays.
        void RenderUI();
        bool HasValidCanvas() const;

        // Create an entity with a specific name,will add TagComponent and TransformComponent by default
        Entity CreateEntity(const std::string &name = std::string());
        // Create an entity with a specific ID (for deserialization) no default components
        Entity CreateEntityWithID(UUID uuid);
        entt::registry &GetRegistry() { return m_Registry; }

        Entity FindEntityByName(std::string_view name);
        Entity GetEntityByUUID(UUID uuid);

        void SetViewportSize(uint32_t width, uint32_t height);

        Entity GetPrimaryCameraEntity();
        glm::mat4 GetPrimaryCameraViewProjectionMatrix();
        glm::vec2 ScreenToWorld(const glm::vec2 &screenPos);

        void OnRuntimeStart();
        void OnRuntimeStop();

        void OnSimulationStart();
        void OnSimulationStop();

        void OnEditorStart();
        void OnEditorStop();

        bool IsRunning() const
        {
            return m_Running;
        };
        bool IsPaused() const
        {
            return m_IsPaused;
        }
        void SetPaused(bool paused)
        {
            m_IsPaused = paused;
        }

        void Step(int frames = 1);

        static Ref<Scene> Copy(Ref<Scene> other);

        void DuplicateEntity(Entity entity);

        // UI relationships are scene-local. Reparenting preserves layout parameters, not screen position.
        Entity CreateCanvas(const std::string &name = "Canvas");
        Entity CreateUIEntity(Entity parent, const std::string &name = "UI Entity");
        bool SetUIParent(Entity child, Entity parent, std::string *error = nullptr);
        bool SetUISiblingOrder(Entity entity, int order);
        Entity DuplicateUISubtree(Entity root);
        // Deferred destruction, safe to request from script callbacks.
        void DestroyEntity(Entity entity);
        // Call only at a safe point with no active registry iteration.
        void FlushPendingEntityDestruction();

    private:
        void RecalculateCameraProjections();

        void SetupPhysicsWorld();
        void DestroyPhysicsWorld();
        void StepPhysicsWorld(Core::Timestep timestep);

        bool OwnsEntity(const Entity &entity) const;

        void RenderScene2D(glm::mat4 viewProjectionMatrix);

    private:
        entt::registry m_Registry;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        Ref<Entity> m_CameraEntity = nullptr;
        b2WorldId worldId;

        bool m_Running = false;
        bool m_IsPaused = false;
        int m_StepFrames = 0;

        float physicsTimeStepAccumulator = 0.0f;

        std::unordered_map<UUID, entt::entity> m_EntityMap;

        friend class Entity;
        friend class EngineEditor::SceneHierarchyPanel;
    };

} // namespace Engine
