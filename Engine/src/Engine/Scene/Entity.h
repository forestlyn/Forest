#pragma once
#include <entt.hpp>
#include <stdexcept>
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

        template <typename T>
        bool CanAddComponent()
        {
            if (!*this) return false;
            if constexpr (std::is_same_v<T, RelationshipComponent>) return false;
            if constexpr (std::is_same_v<T, TransformComponent>) return !HasComponent<RectTransformComponent>();
            if constexpr (std::is_same_v<T, RectTransformComponent>) return !HasComponent<TransformComponent>();
            if constexpr (IsInComponentGroup<T, WorldTransformComponents>) return HasComponent<TransformComponent>();
            if constexpr (IsInComponentGroup<T, RectTransformComponents>) return HasComponent<RectTransformComponent>();
            return true;
        }

        template <typename T>
        bool CanRemoveComponent()
        {
            if (!*this) return false;
            if constexpr (std::is_same_v<T, RelationshipComponent>) return false;
            const auto hasAny = [&]<typename... C>(ComponentGroup<C...>) { return (HasComponent<C>() || ...); };
            if constexpr (std::is_same_v<T, TransformComponent>) return !hasAny(WorldTransformComponents{});
            if constexpr (std::is_same_v<T, RectTransformComponent>)
            {
                if (hasAny(RectTransformComponents{})) return false;
                for (auto child : GetChildren())
                    if (child.HasComponent<RectTransformComponent>()) return false;
            }
            return true;
        }

        template <typename T, typename... Args>
        T &AddComponent(Args &&...args)
        {
            static_assert(!std::is_same_v<T, RelationshipComponent>, "Relationship is managed by Scene");
            if (!CanAddComponent<T>() || HasComponent<T>())
                throw std::logic_error("Cannot add component: duplicate, conflicting transform or missing transform dependency");
            return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
        }

        template <typename T, typename... Args>
        T &AddOrReplaceComponent(Args &&...args)
        {
            static_assert(!std::is_same_v<T, RelationshipComponent>, "Relationship is managed by Scene");
            if (!CanAddComponent<T>())
                throw std::logic_error("Cannot replace component: conflicting transform or missing transform dependency");
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
            if (!CanRemoveComponent<T>())
                throw std::logic_error("Cannot remove a transform required by components or UI children");
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
