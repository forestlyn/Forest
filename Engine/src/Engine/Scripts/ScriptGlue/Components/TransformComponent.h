#pragma once
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scripts/ScriptEngine.h"
namespace Engine
{
    static TransformComponent *FindScriptTransform(UUID entityID)
    {
        auto *scene = ScriptEngine::GetSceneContext();
        if (!scene) return nullptr;
        auto entity = scene->GetEntityByUUID(entityID);
        if (!entity || !entity.HasComponent<TransformComponent>()) return nullptr;
        return &entity.GetComponent<TransformComponent>();
    }
    static bool GetPosition(UUID entityID, glm::vec3 *value)
    {
        if (!value) return false;
        *value = glm::vec3(0);
        auto *transform = FindScriptTransform(entityID);
        if (!transform) return false;
        *value = transform->GetPosition();
        return true;
    }
    static bool SetPosition(UUID entityID, glm::vec3 *value)
    {
        auto *transform = FindScriptTransform(entityID);
        if (!transform || !value) return false;
        transform->SetPosition(*value);
        return true;
    }
    static bool GetRotation(UUID entityID, glm::vec3 *value)
    {
        if (!value) return false;
        *value = glm::vec3(0);
        auto *transform = FindScriptTransform(entityID);
        if (!transform) return false;
        *value = transform->GetRotation();
        return true;
    }
    static bool SetRotation(UUID entityID, glm::vec3 *value)
    {
        auto *transform = FindScriptTransform(entityID);
        if (!transform || !value) return false;
        transform->SetRotation(*value);
        return true;
    }
    static bool GetScale(UUID entityID, glm::vec3 *value)
    {
        if (!value) return false;
        *value = glm::vec3(1);
        auto *transform = FindScriptTransform(entityID);
        if (!transform) return false;
        *value = transform->GetScale();
        return true;
    }
    static bool SetScale(UUID entityID, glm::vec3 *value)
    {
        auto *transform = FindScriptTransform(entityID);
        if (!transform || !value) return false;
        transform->SetScale(*value);
        return true;
    }
}
