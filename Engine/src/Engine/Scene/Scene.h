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

    private:
        entt::registry m_Registry;
        friend class Entity;
    };
} // namespace Engine::Scene