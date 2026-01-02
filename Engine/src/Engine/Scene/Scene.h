#pragma once
#include "Engine/pcheader.h"
#include "Engine/Scene/Component.h"
#include "Engine/Core/Timestep.h"
#include <entt.hpp>

namespace Engine
{
    class Entity;
    class Scene
    {
    public:
        Scene() = default;
        ~Scene() = default;
        void OnUpdate(Core::Timestep deltaTime);

        Entity CreateEntity(const std::string &name = std::string());
        entt::registry &GetRegistry() { return m_Registry; }

        void SetViewportSize(uint32_t width, uint32_t height)
        {
            m_ViewportWidth = width;
            m_ViewportHeight = height;
            RecalculateCameraProjections();
        }

    private:
        void RecalculateCameraProjections();

    private:
        entt::registry m_Registry;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        friend class Entity;
        friend class EngineEditor::SceneHierarchyPanel;
    };
} // namespace Engine