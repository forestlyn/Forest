#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"

namespace Engine
{
    Entity Scene::CreateCanvas(const std::string &name)
    {
        auto entity = CreateEntity(name);
        entity.AddComponent<CanvasComponent>();
        entity.AddComponent<RectTransformComponent>();
        return entity;
    }

    Entity Scene::CreateUIEntity(Entity parent, const std::string &name)
    {
        if (!OwnsEntity(parent) || IsPendingDestruction(parent) || !parent.HasComponent<RectTransformComponent>())
            return {};
        // Creation requires an existing valid Canvas chain.
        auto cursor = parent;
        while (cursor && !cursor.HasComponent<CanvasComponent>())
        {
            if (!cursor.HasComponent<RectTransformComponent>()) return {};
            cursor = GetParent(cursor);
        }
        if (!cursor) return {};
        auto entity = CreateEntity(name);
        entity.AddComponent<RectTransformComponent>();
        if (!SetParent(entity, parent))
        {
            auto &roots = m_ChildrenByParent[UUID(0)];
            std::erase(roots, entity.GetUUID());
            m_EntityMap.erase(entity.GetUUID());
            m_Registry.destroy(entity.m_EntityHandle);
            return {};
        }
        return entity;
    }

    Entity Scene::DuplicateUISubtree(Entity root)
    {
        if (!OwnsEntity(root) || !root.HasComponent<RectTransformComponent>()) return {};
        return DuplicateSubtree(root);
    }
}
