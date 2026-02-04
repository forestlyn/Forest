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

        m_ScriptEngineData->EntityClass = new ScriptClass("Engine", "Entity");
        ENGINE_ASSERT(m_ScriptEngineData->EntityClass);
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
    }

    void ScriptEngine::ShutdownMono()
    {
        mono_assemblies_cleanup();
        mono_domain_unload(m_ScriptEngineData->AppDomain);
        mono_domain_unload(m_ScriptEngineData->RootDomain);
    }

#pragma region ScriptClass Implementation
    ScriptClass::ScriptClass(const std::string &namespaceName, const std::string &className)
    {
        m_Namespace = namespaceName;
        m_ClassName = className;
        m_MonoClass = GetClassFromAssembly(m_ScriptEngineData->CoreAssembly, m_Namespace, m_ClassName);
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
    ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Entity entityID)
    {
        m_ScriptClass = scriptClass;
        m_Instance = m_ScriptClass->Instantiate();

        // Set the entity ID field
        m_Constructor = m_ScriptEngineData->EntityClass->GetMethod(".ctor", 1);
        m_OnCreateMethod = m_ScriptClass->GetMethod("OnCreate", 0);
        m_OnUpdateMethod = m_ScriptClass->GetMethod("OnUpdate", 1);

        {
            void *params = &entityID;
            m_ScriptClass->InvokeMethod(m_Instance, m_Constructor, 1, &params);
        }
    }
#pragma endregion
}
