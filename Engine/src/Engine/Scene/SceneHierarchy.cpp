#include "Scene.h"
#include "Entity.h"
#include "ScriptEntity.h"
#include "Engine/Scripts/ScriptEngine.h"
#include <algorithm>

namespace Engine
{
    bool Scene::OwnsEntity(const Entity &entity) const
    {
        return entity.m_Scene == this && m_Registry.valid(entity.m_EntityHandle);
    }

    bool Scene::IsPendingDestruction(Entity entity) const
    {
        // Also honor legacy callers that mark TagComponent directly.
        while (OwnsEntity(entity))
        {
            if (m_PendingDestruction.contains(entity.GetUUID())) return true;
            const auto *tag = m_Registry.try_get<TagComponent>(entity.m_EntityHandle);
            if (tag && tag->IsRemove()) return true;
            entity = GetParent(entity);
        }
        return false;
    }

    Entity Scene::GetParent(Entity entity) const
    {
        if (!OwnsEntity(entity)) return {};
        const auto *relation = m_Registry.try_get<RelationshipComponent>(entity.m_EntityHandle);
        if (!relation) return {};
        auto it = m_EntityMap.find(relation->GetParent());
        return it != m_EntityMap.end() ? Entity(it->second, const_cast<Scene *>(this)) : Entity{};
    }

    std::vector<Entity> Scene::GetChildren(Entity parent) const
    {
        if (!OwnsEntity(parent)) return {};
        std::vector<Entity> result;
        auto it = m_ChildrenByParent.find(parent.GetUUID());
        if (it != m_ChildrenByParent.end())
            for (auto id : it->second) result.emplace_back(m_EntityMap.at(id), const_cast<Scene *>(this));
        return result;
    }

    std::vector<Entity> Scene::GetRootEntities() const
    {
        std::vector<Entity> result;
        auto it = m_ChildrenByParent.find(UUID(0));
        if (it != m_ChildrenByParent.end())
            for (auto id : it->second) result.emplace_back(m_EntityMap.at(id), const_cast<Scene *>(this));
        return result;
    }

    void Scene::SortChildren(UUID parent)
    {
        auto &list = m_ChildrenByParent[parent];
        std::sort(list.begin(), list.end(), [&](UUID a, UUID b)
        {
            int x = m_Registry.get<RelationshipComponent>(m_EntityMap.at(a)).SiblingOrder;
            int y = m_Registry.get<RelationshipComponent>(m_EntityMap.at(b)).SiblingOrder;
            return x != y ? x < y : uint64_t(a) < uint64_t(b);
        });
    }

    void Scene::InsertChild(UUID parent, UUID child)
    {
        auto &list = m_ChildrenByParent[parent];
        auto position = std::lower_bound(list.begin(), list.end(), child, [&](UUID a, UUID b)
        {
            int x = m_Registry.get<RelationshipComponent>(m_EntityMap.at(a)).SiblingOrder;
            int y = m_Registry.get<RelationshipComponent>(m_EntityMap.at(b)).SiblingOrder;
            return x != y ? x < y : uint64_t(a) < uint64_t(b);
        });
        list.insert(position, child);
    }

    void Scene::RestoreRelationship(Entity entity, UUID parent, int order)
    {
        auto &relation = m_Registry.get<RelationshipComponent>(entity.m_EntityHandle);
        relation.Parent.uuid = parent;
        relation.SiblingOrder = order;
    }

    bool Scene::RebuildHierarchyIndex()
    {
        std::unordered_map<UUID, std::vector<UUID>> children;
        // A completed ancestor chain never needs walking twice.
        std::unordered_set<UUID> complete;
        for (auto [id, handle] : m_EntityMap)
        {
            std::unordered_set<UUID> visiting;
            auto cursor = id;
            while (uint64_t(cursor) && !complete.contains(cursor))
            {
                auto found = m_EntityMap.find(cursor);
                if (found == m_EntityMap.end() || !visiting.insert(cursor).second)
                {
                    ENGINE_ERROR("Invalid hierarchy at entity {}: missing parent or cycle", uint64_t(cursor));
                    return false;
                }
                cursor = m_Registry.get<RelationshipComponent>(found->second).Parent.uuid;
            }
            complete.insert(visiting.begin(), visiting.end());
            children[m_Registry.get<RelationshipComponent>(handle).Parent.uuid].push_back(id);
        }
        m_ChildrenByParent = std::move(children);
        for (auto &[parent, list] : m_ChildrenByParent) SortChildren(parent);
        return true;
    }

    bool Scene::SetParent(Entity child, Entity parent, std::string *error)
    {
        if (error) error->clear();
        const auto reject = [&](const char *message)
        {
            if (error) *error = message;
            return false;
        };
        if (!OwnsEntity(child)) return reject("Child must belong to this scene");
        // Only a default-constructed Entity means detach; a stale/foreign handle is an error.
        const bool detach = parent.m_Scene == nullptr && parent.m_EntityHandle == entt::null;
        if (!detach && !OwnsEntity(parent)) return reject("Parent must belong to this scene");
        if (IsPendingDestruction(child) || (!detach && IsPendingDestruction(parent)))
            return reject("Cannot modify a hierarchy pending deletion");
        for (auto cursor = parent; cursor; cursor = GetParent(cursor))
            if (cursor == child) return reject("Parent would introduce a cycle");
        if (!detach && child.HasComponent<CanvasComponent>()) return reject("Canvas must remain a root");
        if (!detach && child.HasComponent<RectTransformComponent>() && !parent.HasComponent<RectTransformComponent>())
            return reject("RectTransform requires a RectTransform parent");

        auto &relation = m_Registry.get<RelationshipComponent>(child.m_EntityHandle);
        const UUID next = detach ? UUID(0) : parent.GetUUID();
        if (relation.Parent.uuid == next) return true;
        std::erase(m_ChildrenByParent[relation.Parent.uuid], child.GetUUID());
        relation.Parent.uuid = next;
        InsertChild(next, child.GetUUID());
        return true;
    }

    bool Scene::SetSiblingOrder(Entity entity, int order)
    {
        if (!OwnsEntity(entity) || IsPendingDestruction(entity)) return false;
        auto &relation = m_Registry.get<RelationshipComponent>(entity.m_EntityHandle);
        std::erase(m_ChildrenByParent[relation.Parent.uuid], entity.GetUUID());
        relation.SiblingOrder = order;
        InsertChild(relation.Parent.uuid, entity.GetUUID());
        return true;
    }

    uint32_t Scene::GetSiblingIndex(Entity entity) const
    {
        if (!OwnsEntity(entity)) return 0;
        auto parent = m_Registry.get<RelationshipComponent>(entity.m_EntityHandle).Parent.uuid;
        auto it = m_ChildrenByParent.find(parent);
        if (it == m_ChildrenByParent.end()) return 0;
        return uint32_t(std::find(it->second.begin(), it->second.end(), entity.GetUUID()) - it->second.begin());
    }

    bool Scene::SetSiblingIndex(Entity entity, uint32_t index)
    {
        if (!OwnsEntity(entity) || IsPendingDestruction(entity)) return false;
        auto parent = m_Registry.get<RelationshipComponent>(entity.m_EntityHandle).Parent.uuid;
        auto &list = m_ChildrenByParent[parent];
        std::erase(list, entity.GetUUID());
        list.insert(list.begin() + std::min(size_t(index), list.size()), entity.GetUUID());
        // Index is a position; order is a persisted sort key. Normalize only for index moves.
        for (size_t i = 0; i < list.size(); ++i)
            m_Registry.get<RelationshipComponent>(m_EntityMap.at(list[i])).SiblingOrder = int(i);
        return true;
    }

    Entity Scene::DuplicateSubtree(Entity root)
    {
        if (!OwnsEntity(root) || IsPendingDestruction(root)) return {};
        std::vector<Entity> nodes{root};
        for (size_t i = 0; i < nodes.size(); ++i)
        {
            if (IsPendingDestruction(nodes[i])) return {};
            auto children = GetChildren(nodes[i]);
            nodes.insert(nodes.end(), children.begin(), children.end());
        }
        std::unordered_map<UUID, Entity> copies;
        for (auto source : nodes)
        {
            auto copy = CreateEntity(source.HasComponent<TagComponent>() ? source.GetName() + "_Copy" : "Copy");
            [&]<typename... T>(ComponentGroup<T...>)
            {
                ([&]
                {
                    if constexpr (!std::is_same_v<T, RelationshipComponent>)
                        if (source.HasComponent<T>()) copy.AddOrReplaceComponent<T>(source.GetComponent<T>());
                }(), ...);
            }(AllComponents{});
            if (copy.HasComponent<NativeScriptComponent>()) copy.GetComponent<NativeScriptComponent>().Instance = nullptr;
            if (copy.HasComponent<Rigidbody2DComponent>()) copy.GetComponent<Rigidbody2DComponent>().RuntimeBodyId = {};
            copies.emplace(source.GetUUID(), copy);
        }
        for (auto source : nodes)
        {
            auto copy = copies.at(source.GetUUID());
            const auto &relation = source.GetComponent<RelationshipComponent>();
            auto parent = GetParent(source);
            if (copies.contains(relation.GetParent())) parent = copies.at(relation.GetParent());
            SetParent(copy, parent);
            SetSiblingOrder(copy, relation.GetSiblingOrder());
        }
        return copies.at(root.GetUUID());
    }

    void Scene::DestroyEntity(Entity entity)
    {
        if (!OwnsEntity(entity)) return;
        std::vector<Entity> stack{entity};
        while (!stack.empty())
        {
            auto node = stack.back();
            stack.pop_back();
            if (!m_PendingDestruction.insert(node.GetUUID()).second) continue;
            if (node.HasComponent<TagComponent>()) node.GetComponent<TagComponent>().SetRemove(true);
            auto children = GetChildren(node);
            stack.insert(stack.end(), children.begin(), children.end());
        }
    }

    void Scene::DestroyChildren(Entity entity)
    {
        for (auto child : GetChildren(entity)) DestroyEntity(child);
    }

    void Scene::FlushPendingEntityDestruction()
    {
        if (m_FlushingDestruction) return;
        for (auto e : m_Registry.view<TagComponent>())
            if (m_Registry.get<TagComponent>(e).IsRemove()) DestroyEntity(Entity(e, this));
        if (m_PendingDestruction.empty()) return;
        m_FlushingDestruction = true;
        // Collect a stable parent-first traversal, then destroy in reverse. Callbacks may
        // enqueue other branches; those requests remain pending for the next safe point.
        std::vector<Entity> order;
        for (auto id : m_PendingDestruction)
        {
            auto it = m_EntityMap.find(id);
            if (it == m_EntityMap.end()) continue;
            Entity entity(it->second, this);
            auto parent = GetParent(entity);
            if (!parent || !m_PendingDestruction.contains(parent.GetUUID())) order.push_back(entity);
        }
        for (size_t i = 0; i < order.size(); ++i)
        {
            auto children = GetChildren(order[i]);
            order.insert(order.end(), children.begin(), children.end());
        }
        for (auto it = order.rbegin(); it != order.rend(); ++it)
        {
            auto entity = *it;
            const auto id = entity.GetUUID();
            if (entity.HasComponent<NativeScriptComponent>())
            {
                auto &script = entity.GetComponent<NativeScriptComponent>();
                if (script.Instance)
                {
                    script.Instance->OnDestroy();
                    if (script.Destroy) script.Destroy(&script);
                }
            }
            ScriptEngine::ReleaseEntityInstance(this, id);
            if (entity.HasComponent<Rigidbody2DComponent>())
            {
                auto body = entity.GetComponent<Rigidbody2DComponent>().RuntimeBodyId;
                if (b2Body_IsValid(body)) b2DestroyBody(body);
            }
            if (m_CameraEntity && *m_CameraEntity == entity) m_CameraEntity.reset();
            auto parent = entity.GetComponent<RelationshipComponent>().GetParent();
            std::erase(m_ChildrenByParent[parent], id);
            m_ChildrenByParent.erase(id);
            m_PendingDestruction.erase(id);
            m_EntityMap.erase(id);
            m_Registry.destroy(entity.m_EntityHandle);
        }
        m_FlushingDestruction = false;
    }
}
