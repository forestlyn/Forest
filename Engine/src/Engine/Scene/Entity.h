#pragma once
#include <entt.hpp>
#include "Engine/Scene/Scene.h"
namespace Engine
{
    class Entity
    {
    private:
        friend class Scene;
        entt::entity m_EntityHandle{entt::null};
        Scene *m_Scene = nullptr;

    public:
        Entity() = default;
        Entity(entt::entity handle, Scene *scene);

        template <typename T, typename... Args>
        T &AddComponent(Args &&...args)
        {
            static_assert(!std::is_same_v<T, RelationshipComponent>, "Relationship is managed by Scene");
            return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
        }

        template <typename T, typename... Args>
        T &AddOrReplaceComponent(Args &&...args)
        {
            static_assert(!std::is_same_v<T, RelationshipComponent>, "Relationship is managed by Scene");
            return m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
        }

        template <typename T>
        decltype(auto) GetComponent()
        {
            if constexpr (std::is_same_v<T, RelationshipComponent>)
                return std::as_const(m_Scene->m_Registry.get<T>(m_EntityHandle));
            else
                return m_Scene->m_Registry.get<T>(m_EntityHandle);
        }

        template <typename T>
        bool HasComponent()
        {
            return m_Scene->m_Registry.any_of<T>(m_EntityHandle);
        }

        template <typename T>
        void RemoveComponent()
        {
            static_assert(!std::is_same_v<T, RelationshipComponent>, "Relationship is managed by Scene");
            m_Scene->m_Registry.remove<T>(m_EntityHandle);
        }

        operator bool() const { return m_Scene && m_Scene->m_Registry.valid(m_EntityHandle); }
        operator entt::entity() const { return m_EntityHandle; }
        bool operator==(const Entity &other) const
        {
            return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
        }
        bool operator!=(const Entity &other) const
        {
            return !(*this == other);
        }
        operator uint32_t() const { return (uint32_t)m_EntityHandle; }

        UUID GetUUID() const;
        const std::string &GetName();

        // RelationShip Related
        Entity GetParent();
        bool SetParent(Entity parent, std::string *error = nullptr);
        bool Detach();
        uint32_t GetSiblingIndex();
        bool SetSiblingIndex(uint32_t index);
        std::vector<Entity> GetChildren();
        void Destroy();
        void DestroyChildren();
    };
}
