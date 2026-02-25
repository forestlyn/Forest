#pragma once
#include "Engine/pcheader.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scripts/ScriptEngine.h"
namespace Engine
{
    static uint64_t FindEntityByName(MonoString *name)
    {
        Scene *scene = ScriptEngine::GetSceneContext();
        if (scene == nullptr)
        {
            LOG_ERROR("No active scene found!");
            return 0;
        }
        return scene->FindEntityByName(mono_string_to_utf8(name)).GetUUID();
    }

    static MonoObject *GetScriptInstance(UUID entityID)
    {
        return ScriptEngine::GetManagedInstance(entityID);
    }
}