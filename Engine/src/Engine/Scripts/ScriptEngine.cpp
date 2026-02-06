#include "ScriptEngine.h"
#include "Engine/pcheader.h"
#include "Engine/Core/Core.h"
#include "Engine/Core/Application.h"
#include "ScriptGlue.h"
#include "Utils.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/attrdefs.h>
#include <fstream>
namespace Engine
{
    struct ScriptEngineData
    {
        MonoDomain *RootDomain = nullptr;
        MonoDomain *AppDomain = nullptr;

        MonoAssembly *CoreAssembly = nullptr;
        MonoImage *CoreAssemblyImage = nullptr;

        MonoAssembly *AppAssembly = nullptr;
        MonoImage *AppAssemblyImage = nullptr;

        std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;
        std::unordered_map<UUID, Ref<ScriptInstance>> EntityInstances;

        ScriptClass *EntityClass = nullptr;
        // Runtime
        Scene *SceneContext = nullptr;
    };
    static ScriptEngineData *m_ScriptEngineData = nullptr;

    void ScriptEngine::Init()
    {
        m_ScriptEngineData = new ScriptEngineData();

        InitMono();

        ScriptGlue::RegisterFuncs();
        ScriptGlue::RegisterComponents();

        m_ScriptEngineData->EntityClass = new ScriptClass("Engine", "Entity", true);
        ENGINE_ASSERT(m_ScriptEngineData->EntityClass);

        LoadAssemblyClasses(m_ScriptEngineData->CoreAssembly);
        LoadAssemblyClasses(m_ScriptEngineData->AppAssembly);
    }

    void ScriptEngine::Shutdown()
    {
        ShutdownMono();
        delete m_ScriptEngineData;
    }

    Scene *ScriptEngine::GetSceneContext()
    {
        return m_ScriptEngineData->SceneContext;
    }

    MonoImage *ScriptEngine::GetCoreAssemblyImage()
    {
        return m_ScriptEngineData->CoreAssemblyImage;
    }

    void ScriptEngine::OnRuntimeStart(Scene *scene)
    {
        m_ScriptEngineData->SceneContext = scene;
    }

    void ScriptEngine::OnCreateEntity(Entity entity)
    {
        std::string scriptClassName = entity.GetComponent<ScriptComponent>().ScriptClassName;
        if (EntityClassExists(scriptClassName))
        {
            ENGINE_INFO("Creating script instance for entity '{}' (id:{}) with class '{}'", entity.GetName(), (uint64_t)entity.GetUUID(), scriptClassName);
            Ref<ScriptClass> scriptClass = m_ScriptEngineData->EntityClasses[scriptClassName];
            Ref<ScriptInstance> instance = CreateRef<ScriptInstance>(scriptClass, entity);
            m_ScriptEngineData->EntityInstances[entity.GetComponent<IDComponent>().ID] = instance;
            instance->InvokeOnCreate();
        }
        else
        {
            ENGINE_WARN("Script class '{}' not found for entity '{}'", scriptClassName, entity.GetName());
        }
    }

    void ScriptEngine::OnUpdateEntity(Entity entity, Core::Timestep ts)
    {
        UUID entityID = entity.GetComponent<IDComponent>().ID;
        ENGINE_ASSERT(m_ScriptEngineData->EntityInstances.find(entityID) != m_ScriptEngineData->EntityInstances.end(), "Script instance not found for entity!");
        Ref<ScriptInstance> instance = m_ScriptEngineData->EntityInstances[entityID];
        instance->InvokeOnUpdate(ts.GetSeconds());
    }

    bool ScriptEngine::EntityClassExists(const std::string &className)
    {
        return m_ScriptEngineData->EntityClasses.find(className) != m_ScriptEngineData->EntityClasses.end();
    }

    void ScriptEngine::InitMono()
    {
        Core::Application &app = Core::Application::Get();
        mono_set_assemblies_path(app.GetSpecification().MonoAssemblyPath.c_str());

        MonoDomain *rootDomain = mono_jit_init("EngineJITRuntime");
        ENGINE_ASSERT(rootDomain);
        m_ScriptEngineData->RootDomain = rootDomain;

        char domainName[] = "EngineScriptRuntime";
        MonoDomain *appDomain = mono_domain_create_appdomain(domainName, nullptr);
        ENGINE_ASSERT(appDomain);
        mono_domain_set(appDomain, true);
        m_ScriptEngineData->AppDomain = appDomain;

        m_ScriptEngineData->CoreAssembly = LoadCSharpAssembly("scripts/bin/Engine-ScriptCore.dll");
        m_ScriptEngineData->CoreAssemblyImage = mono_assembly_get_image(m_ScriptEngineData->CoreAssembly);
        ENGINE_ASSERT(m_ScriptEngineData->CoreAssemblyImage);
        PrintAssemblyTypes(m_ScriptEngineData->CoreAssembly);

        m_ScriptEngineData->AppAssembly = LoadCSharpAssembly("scripts/bin/Sandbox.dll");
        m_ScriptEngineData->AppAssemblyImage = mono_assembly_get_image(m_ScriptEngineData->AppAssembly);
        ENGINE_ASSERT(m_ScriptEngineData->AppAssemblyImage);
    }

    void ScriptEngine::ShutdownMono()
    {
        mono_assemblies_cleanup();
        mono_domain_unload(m_ScriptEngineData->AppDomain);
        mono_domain_unload(m_ScriptEngineData->RootDomain);
    }

    void ScriptEngine::LoadAssemblyClasses(MonoAssembly *assembly)
    {
        MonoImage *image = mono_assembly_get_image(assembly);
        const MonoTableInfo *typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
        int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

        for (int32_t i = 0; i < numTypes; i++)
        {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

            const char *nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
            const char *name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

            std::string fullClassName = std::string(nameSpace) + "." + std::string(name);

            MonoClass *monoClass = mono_class_from_name(image, nameSpace, name);
            if (!monoClass)
                continue;
            if (fullClassName == "Engine.Entity")
                continue;
            if (mono_class_is_subclass_of(monoClass, m_ScriptEngineData->EntityClass->GetMonoClass(), false) == false)
                continue;
            Ref<ScriptClass> scriptClass = CreateRef<ScriptClass>(nameSpace, name);
            m_ScriptEngineData->EntityClasses[fullClassName] = scriptClass;
            ENGINE_INFO("Loaded script class: {}", fullClassName);
        }
    }

#pragma region ScriptClass Implementation
    ScriptClass::ScriptClass(const std::string &namespaceName, const std::string &className, bool isCore)
    {
        m_Namespace = namespaceName;
        m_ClassName = className;
        MonoAssembly *assembly = isCore ? m_ScriptEngineData->CoreAssembly : m_ScriptEngineData->AppAssembly;
        m_MonoClass = GetClassFromAssembly(assembly, m_Namespace, m_ClassName);
    }

    ScriptClass::~ScriptClass()
    {
    }

    MonoObject *ScriptClass::Instantiate()
    {
        return InitializeClass(m_ScriptEngineData->AppDomain, m_MonoClass);
    }

    MonoMethod *ScriptClass::GetMethod(const std::string &methodName, int paramCount)
    {
        return GetMethodFromClass(m_MonoClass, methodName, paramCount);
    }

    void ScriptClass::InvokeMethod(MonoObject *instance, const std::string &methodName, int paramCount, void **params)
    {
        MonoMethod *method = GetMethod(methodName, paramCount);
        MonoObject *exception = nullptr;
        mono_runtime_invoke(method, instance, params, &exception);
        CheckException(exception);
    }

    void ScriptClass::InvokeMethod(MonoObject *instance, MonoMethod *method, int paramCount, void **params)
    {
        MonoObject *exception = nullptr;
        mono_runtime_invoke(method, instance, params, &exception);
        CheckException(exception);
    }

#pragma endregion

#pragma region ScriptInstance Implementation
    ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity)
    {
        m_ScriptClass = scriptClass;
        m_Instance = m_ScriptClass->Instantiate();

        // Set the entity ID field
        m_Constructor = m_ScriptEngineData->EntityClass->GetMethod(".ctor", 1);
        m_OnCreateMethod = m_ScriptClass->GetMethod("OnCreate", 0);
        m_OnUpdateMethod = m_ScriptClass->GetMethod("OnUpdate", 1);

        {
            UUID entityID = entity.GetUUID();
            void *params = &entityID;
            m_ScriptClass->InvokeMethod(m_Instance, m_Constructor, 1, &params);
        }
    }
    void ScriptInstance::InvokeOnCreate()
    {
        m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod);
    }
    void ScriptInstance::InvokeOnUpdate(float deltaTime)
    {
        void *params = &deltaTime;
        m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, 1, &params);
    }
#pragma endregion
}
