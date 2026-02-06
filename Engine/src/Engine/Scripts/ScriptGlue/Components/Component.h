#include "Engine/Scripts/ScriptEngine.h"
#include "Engine/Scene/Scene.h"
#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>
#include <mono/metadata/assembly.h>
namespace Engine
{
    static std::unordered_map<MonoType *, std::function<bool(Entity)>> s_EntityHasComponentFuncs;
    static std::unordered_map<MonoType *, std::function<void(Entity)>> s_EntityRemoveComponentFuncs;
    static std::unordered_map<MonoType *, std::function<void(Entity)>> s_EntityAddComponentFuncs;

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

    static void AddComponent(UUID entityID, MonoReflectionType *componentType)
    {
        Scene *scene = ScriptEngine::GetSceneContext();
        ENGINE_ASSERT(scene);
        Entity entity = scene->GetEntityByUUID(entityID);
        ENGINE_ASSERT(entity);

        MonoType *managedType = mono_reflection_type_get_type(componentType);
        ENGINE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end());
        s_EntityAddComponentFuncs.at(managedType)(entity);
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
            s_EntityAddComponentFuncs[managedType] = [](Entity entity)
            {
                entity.AddComponent<T>();
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
}