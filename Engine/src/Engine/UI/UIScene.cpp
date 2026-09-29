#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"
#include "UILayout.h"
#include <unordered_set>

namespace Engine
{
    bool Scene::OwnsEntity(const Entity &entity) const
    {
        return entity.m_Scene == this && m_Registry.valid(entity.m_EntityHandle);
    }

    Entity Scene::CreateCanvas(const std::string &name)
    {
        auto entity = CreateEntity(name);
        entity.AddComponent<CanvasComponent>();
        entity.AddComponent<RectTransformComponent>();
        return entity;
    }

    Entity Scene::CreateUIEntity(Entity parent, const std::string &name)
    {
        if (!OwnsEntity(parent) || !parent.HasComponent<RectTransformComponent>()) return {};
        auto entity = CreateEntity(name);
        entity.AddComponent<RectTransformComponent>();
        if (!SetUIParent(entity, parent))
        {
            m_EntityMap.erase(entity.GetUUID());
            m_Registry.destroy(entity.m_EntityHandle);
            return {};
        }
        return entity;
    }

    bool Scene::SetUIParent(Entity child, Entity parent, std::string *error)
    {
        if (error) error->clear();
        const auto reject = [&](const char *message)
        {
            if (error) *error = message;
            return false;
        };
        if (!OwnsEntity(child) || !OwnsEntity(parent)) return reject("UI parent and child must belong to this scene");
        if (!child.HasComponent<RectTransformComponent>() || !parent.HasComponent<RectTransformComponent>())
            return reject("UI parent and child require RectTransformComponent");
        if (child.HasComponent<CanvasComponent>()) return reject("Canvas must remain a root");
        if (child.HasComponent<TagComponent>() && child.GetComponent<TagComponent>().IsRemove())
            return reject("Cannot reparent a node pending deletion");
        std::unordered_set<uint64_t> visited;
        Entity cursor = parent;
        while (true)
        {
            if (cursor == child) return reject("UI parent would introduce a cycle");
            if (!visited.insert(uint64_t(cursor.GetUUID())).second) return reject("Parent hierarchy contains a cycle");
            if (cursor.HasComponent<TagComponent>() && cursor.GetComponent<TagComponent>().IsRemove())
                return reject("UI parent hierarchy is pending deletion");
            const auto &rect = cursor.GetComponent<RectTransformComponent>();
            if (cursor.HasComponent<CanvasComponent>())
            {
                if (uint64_t(rect.Parent.uuid)) return reject("Nested Canvas is not supported");
                break;
            }
            auto it = m_EntityMap.find(rect.Parent.uuid);
            if (it == m_EntityMap.end() || !m_Registry.valid(it->second) ||
                !m_Registry.all_of<RectTransformComponent>(it->second))
                return reject("UI parent hierarchy has no valid root Canvas");
            cursor = Entity(it->second, this);
        }
        child.GetComponent<RectTransformComponent>().Parent.uuid = parent.GetUUID();
        return true;
    }

    bool Scene::SetUISiblingOrder(Entity entity, int order)
    {
        if (!OwnsEntity(entity) || !entity.HasComponent<RectTransformComponent>()) return false;
        entity.GetComponent<RectTransformComponent>().SiblingOrder = order;
        return true;
    }

    Entity Scene::DuplicateUISubtree(Entity root)
    {
        if (!OwnsEntity(root) || !root.HasComponent<RectTransformComponent>()) return {};
        const auto hierarchy = UI::BuildHierarchy(m_Registry);
        std::unordered_set<uint64_t> included;
        std::vector<UI::HierarchyNode> nodes;
        for (const auto &node : hierarchy.Nodes)
            if (node.ID == root.GetUUID() || included.contains(uint64_t(node.Parent)))
            {
                if (m_Registry.all_of<TagComponent>(node.Entity) && m_Registry.get<TagComponent>(node.Entity).IsRemove())
                    return {};
                included.insert(uint64_t(node.ID));
                nodes.push_back(node);
            }
        if (nodes.empty()) return {};
        std::unordered_map<uint64_t, Entity> copies;
        for (const auto &node : nodes)
        {
            Entity source(node.Entity, this);
            auto copy = CreateEntity(source.GetName() + "_Copy");
            [&]<typename... T>(ComponentGroup<T...>)
            {
                ([&] { if (source.HasComponent<T>()) copy.AddOrReplaceComponent<T>(source.GetComponent<T>()); }(), ...);
            }(AllComponents{});
            if (copy.HasComponent<NativeScriptComponent>())
                copy.GetComponent<NativeScriptComponent>().Instance = nullptr;
            copies.emplace(uint64_t(node.ID), copy);
        }
        for (auto &[oldID, copy] : copies)
        {
            auto &parent = copy.GetComponent<RectTransformComponent>().Parent;
            auto it = copies.find(uint64_t(parent.uuid));
            if (it != copies.end()) parent.uuid = it->second.GetUUID();
        }
        return copies.at(uint64_t(root.GetUUID()));
    }

    void Scene::DestroyEntity(Entity entity)
    {
        if (OwnsEntity(entity) && entity.HasComponent<TagComponent>()) entity.GetComponent<TagComponent>().SetRemove(true);
    }
}
