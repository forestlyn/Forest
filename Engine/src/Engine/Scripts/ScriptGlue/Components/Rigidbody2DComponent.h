#pragma once
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"
#include "Engine/pcheader.h"
#include "box2d/box2d.h"
namespace Engine
{

    static void ApplyLinearImpulse(UUID entityID, glm::vec2 *impulse, glm::vec2 *point, bool wake)
    {
        Scene *scene = ScriptEngine::GetSceneContext();
        ENGINE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        ENGINE_ASSERT(entity);
        Rigidbody2DComponent &rb2d = entity.GetComponent<Rigidbody2DComponent>();
        b2Body_ApplyLinearImpulse(rb2d.RuntimeBodyId, b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
    }

    static void ApplyLinearImpulseToCenter(UUID entityID, glm::vec2 *impulse, bool wake)
    {
        Scene *scene = ScriptEngine::GetSceneContext();
        ENGINE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        ENGINE_ASSERT(entity);

        Rigidbody2DComponent &rb2d = entity.GetComponent<Rigidbody2DComponent>();
        b2Body_ApplyLinearImpulseToCenter(rb2d.RuntimeBodyId, b2Vec2(impulse->x, impulse->y), wake);
    }
}