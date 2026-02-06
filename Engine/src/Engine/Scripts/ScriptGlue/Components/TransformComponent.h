#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"
#include "Engine/pcheader.h"
namespace Engine
{
    static void GetPosition(UUID entityID, glm::vec3 *outPosition)
    {
        Scene *scene = ScriptEngine::GetSceneContext();
        ENGINE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        ENGINE_ASSERT(entity);

        *outPosition = entity.GetComponent<TransformComponent>().GetPosition();
    }

    static void SetPosition(UUID entityID, glm::vec3 *position)
    {
        Scene *scene = ScriptEngine::GetSceneContext();
        ENGINE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        ENGINE_ASSERT(entity);
        entity.GetComponent<TransformComponent>().SetPosition(*position);
    }

    static void GetRotation(UUID entityID, glm::vec3 *outRotation)
    {
        Scene *scene = ScriptEngine::GetSceneContext();
        ENGINE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        ENGINE_ASSERT(entity);

        *outRotation = entity.GetComponent<TransformComponent>().GetRotation();
    }

    static void SetRotation(UUID entityID, glm::vec3 *rotation)
    {
        Scene *scene = ScriptEngine::GetSceneContext();
        ENGINE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        ENGINE_ASSERT(entity);

        entity.GetComponent<TransformComponent>().SetRotation(*rotation);
    }

    static void GetScale(UUID entityID, glm::vec3 *outScale)
    {
        Scene *scene = ScriptEngine::GetSceneContext();
        ENGINE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        ENGINE_ASSERT(entity);

        *outScale = entity.GetComponent<TransformComponent>().GetScale();
    }

    static void SetScale(UUID entityID, glm::vec3 *scale)
    {
        Scene *scene = ScriptEngine::GetSceneContext();
        ENGINE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        ENGINE_ASSERT(entity);

        entity.GetComponent<TransformComponent>().SetScale(*scale);
    }
}