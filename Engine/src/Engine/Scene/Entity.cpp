#include "Entity.h"

namespace Engine
{
    Entity::Entity(entt::entity handle, Scene *scene)
        : m_EntityHandle(handle), m_Scene(scene)
    {
    }
    UUID Entity::GetUUID()
    {
        return m_Scene->m_Registry.get<IDComponent>(m_EntityHandle).ID;
    }
}