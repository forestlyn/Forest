#include "ScriptGlue.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Input.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scripts/ScriptEngine.h"
#include <iostream>
#include <glm/glm.hpp>
#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>
#include <mono/metadata/assembly.h>
#include "Engine/Scene/Component.h"
namespace Engine
{

    static std::unordered_map<MonoType *, std::function<bool(Entity)>> s_EntityHasComponentFuncs;
    static std::unordered_map<MonoType *, std::function<void(Entity)>> s_EntityRemoveComponentFuncs;

#define ADD_ENGINE_INTERNAL_CALL(namespace, func_name) mono_add_internal_call("Engine." #namespace "::" #func_name, func_name)

    static void NativeLog(MonoString *string)
    {
        char *cStr = mono_string_to_utf8(string);
        std::string str(cStr);
        mono_free(cStr);
        LOG_INFO("{0}", str);
    }

    static void NativeLogWarn(MonoString *string)
    {
        char *cStr = mono_string_to_utf8(string);
        std::string str(cStr);
        mono_free(cStr);
        LOG_WARN("{0}", str);
    }

    static void NativeLogError(MonoString *string)
    {
        char *cStr = mono_string_to_utf8(string);
        std::string str(cStr);
        mono_free(cStr);
        LOG_ERROR("{0}", str);
    }

    static bool IsKeyPressed(int keycode)
    {
        return Core::Input::IsKeyPressed(keycode);
    }

    static bool HasComponent(UUID entityID, MonoReflectionType *componentType)
    {
        Scene *scene = ScriptEngine::GetSceneContext();
        ENGINE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        ENGINE_ASSERT(entity);

        MonoType *managedType = mono_reflection_type_get_type(componentType);
        ENGINE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end());
        return s_EntityHasComponentFuncs.at(managedType)(entity);
    }

    static void RemoveComponent(UUID entityID, MonoReflectionType *componentType)
    {
        Scene *scene = ScriptEngine::GetSceneContext();
        ENGINE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        ENGINE_ASSERT(entity);

        MonoType *managedType = mono_reflection_type_get_type(componentType);
        ENGINE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end());
        s_EntityRemoveComponentFuncs.at(managedType)(entity);
    }

    template <typename... Component>
    void RegisterComponent()
    {
        ([&]<typename T>()
         {
            std::string_view typeName = typeid(Component).name();
			size_t pos = typeName.find_last_of(':');
			std::string_view structName = typeName.substr(pos + 1);
			std::string managedTypename = fmt::format("Engine.{}", structName);

			MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());
            if(!managedType)
            {
                ENGINE_ERROR("Failed to find managed type for component: {}", managedTypename);
                return;
            }
            s_EntityHasComponentFuncs[managedType] = [](Entity entity)
            {
                return entity.HasComponent<T>();
            }; 
            s_EntityRemoveComponentFuncs[managedType] = [](Entity entity)
            {
                entity.RemoveComponent<T>();
            }; }.template operator()<Component>(),
         ...);
    }
    template <typename... Component>
    void RegisterComponent(ComponentGroup<Component...> group)
    {
        RegisterComponent<Component...>();
    }

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

        ADD_ENGINE_INTERNAL_CALL(ComponentInternalCall, HasComponent);
        ADD_ENGINE_INTERNAL_CALL(ComponentInternalCall, RemoveComponent);
    }

}