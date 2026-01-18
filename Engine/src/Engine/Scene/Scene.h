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
        Scene();
        ~Scene()
        {
            LOG_INFO("Scene destroyed.");
        }
        void OnUpdateRuntime(Engine::Core::Timestep timestep);
        void OnUpdateEditor(Engine::Core::Timestep timestep, Renderer::EditorCamera &editorCamera);

        // Create an entity with a specific name,will add TagComponent and TransformComponent by default
        Entity CreateEntity(const std::string &name = std::string());
        // Create an entity with a specific ID (for deserialization) no default components
        Entity CreateEntityWithID(UUID uuid, const std::string &name = std::string());
        entt::registry &GetRegistry() { return m_Registry; }

        void SetViewportSize(uint32_t width, uint32_t height);

        Entity GetPrimaryCameraEntity();
        glm::vec2 ScreenToWorld(const glm::vec2 &screenPos);

        void OnRuntimeStart();
        void OnRuntimeStop();

    private:
        void RecalculateCameraProjections();

    private:
        entt::registry m_Registry;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        Ref<Entity> m_CameraEntity = nullptr;
        b2WorldId worldId;

        float physicsTimeStepAccumulator = 0.0f;

        friend class Entity;
        friend class EngineEditor::SceneHierarchyPanel;
    };
} // namespace Engine