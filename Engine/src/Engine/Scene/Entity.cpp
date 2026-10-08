#include "Entity.h"

namespace Engine
{
    Entity::Entity(entt::entity handle, Scene *scene)
        : m_EntityHandle(handle), m_Scene(scene)
    {
    }
    UUID Entity::GetUUID() const
    {
        return m_Scene->m_Registry.get<IDComponent>(m_EntityHandle).ID;
    }
    const std::string &Entity::GetName()
    {
        return m_Scene->m_Registry.get<TagComponent>(m_EntityHandle).Tag;
    }
    Entity Entity::GetParent() { return m_Scene ? m_Scene->GetParent(*this) : Entity{}; }
    bool Entity::SetParent(Entity parent, std::string *error) { return m_Scene && m_Scene->SetParent(*this, parent, error); }
    bool Entity::Detach() { return SetParent({}); }
    uint32_t Entity::GetSiblingIndex() { return m_Scene ? m_Scene->GetSiblingIndex(*this) : 0; }
    bool Entity::SetSiblingIndex(uint32_t index) { return m_Scene && m_Scene->SetSiblingIndex(*this, index); }
    std::vector<Entity> Entity::GetChildren() { return m_Scene ? m_Scene->GetChildren(*this) : std::vector<Entity>{}; }
    void Entity::Destroy() { if (m_Scene) m_Scene->DestroyEntity(*this); }
    void Entity::DestroyChildren() { if (m_Scene) m_Scene->DestroyChildren(*this); }
}