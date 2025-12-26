#pragma once
#include <entt.hpp>
#include "Engine/pcheader.h"
#include "Engine/Scene/Component.h"
#include "Engine/Core/Timestep.h"
namespace Engine::Scene
{
    class Scene
    {
    public:
        Scene() = default;
        ~Scene() = default;
        void OnUpdate(Core::Timestep deltaTime);

        entt::registry &GetRegistry() { return m_Registry; }

    private:
        entt::registry m_Registry;
    };
} // namespace Engine::Scene