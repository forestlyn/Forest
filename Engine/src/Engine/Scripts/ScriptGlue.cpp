#include "ScriptGlue.h"
#include "Engine/Scripts/ScriptEngine.h"
#include <iostream>
#include <glm/glm.hpp>
#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>
#include <mono/metadata/assembly.h>
#include "Engine/Scene/Component.h"
#include "ScriptGlue/Log.h"
#include "ScriptGlue/Input.h"
#include "ScriptGlue/Entity.h"
#include "ScriptGlue/Components/Component.h"
#include "ScriptGlue/Components/TransformComponent.h"
#include "ScriptGlue/Components/Rigidbody2DComponent.h"
namespace Engine
{

#define ADD_ENGINE_INTERNAL_CALL(namespace, func_name) mono_add_internal_call("Engine." #namespace "::" #func_name, func_name)

    void ScriptGlue::RegisterComponents()
    {
        RegisterComponent(AllComponents{});
    }

    void ScriptGlue::RegisterFuncs()
    {
        // Registration logic for all scriptable classes goes here.
        ADD_ENGINE_INTERNAL_CALL(Log, NativeLog);
        ADD_ENGINE_INTERNAL_CALL(Log, NativeLogWarn);
        ADD_ENGINE_INTERNAL_CALL(Log, NativeLogError);

        ADD_ENGINE_INTERNAL_CALL(Input, IsKeyPressed);

        ADD_ENGINE_INTERNAL_CALL(ComponentInternalCalls, HasComponent);
        ADD_ENGINE_INTERNAL_CALL(ComponentInternalCalls, AddComponent);
        ADD_ENGINE_INTERNAL_CALL(ComponentInternalCalls, RemoveComponent);

        ADD_ENGINE_INTERNAL_CALL(TransformComponentInternalCalls, GetPosition);
        ADD_ENGINE_INTERNAL_CALL(TransformComponentInternalCalls, SetPosition);
        ADD_ENGINE_INTERNAL_CALL(TransformComponentInternalCalls, GetRotation);
        ADD_ENGINE_INTERNAL_CALL(TransformComponentInternalCalls, SetRotation);
        ADD_ENGINE_INTERNAL_CALL(TransformComponentInternalCalls, GetScale);
        ADD_ENGINE_INTERNAL_CALL(TransformComponentInternalCalls, SetScale);

        ADD_ENGINE_INTERNAL_CALL(Rigidbody2DComponentInternalCalls, ApplyLinearImpulse);
        ADD_ENGINE_INTERNAL_CALL(Rigidbody2DComponentInternalCalls, ApplyLinearImpulseToCenter);

        ADD_ENGINE_INTERNAL_CALL(EntityInternalCalls, FindEntityByName);
        ADD_ENGINE_INTERNAL_CALL(EntityInternalCalls, GetScriptInstance);
    }
}