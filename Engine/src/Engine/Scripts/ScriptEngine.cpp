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
#include <filesystem>
#include <FileWatch.hpp>
namespace Engine
{
    struct ScriptEngineData
    {
        MonoDomain *RootDomain = nullptr;
        MonoDomain *AppDomain = nullptr;

        MonoAssembly *CoreAssembly = nullptr;
        MonoImage *CoreAssemblyImage = nullptr;
        std::filesystem::path CoreAssemblyPath;

        MonoAssembly *AppAssembly = nullptr;
        MonoImage *AppAssemblyImage = nullptr;
        std::filesystem::path AppAssemblyPath;

        Scope<filewatch::FileWatch<std::string>> AppAssemblyFileWatcher;
        bool IsReloadingAssembly = false;

        std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;
        std::unordered_map<UUID, Ref<ScriptInstance>> EntityInstances;
        std::unordered_map<UUID, ScriptFieldMap> EntityFieldMaps;

        ScriptClass *EntityClass = nullptr;
        // Runtime
        Scene *SceneContext = nullptr;
    };
    static ScriptEngineData *m_ScriptEngineData = nullptr;

#pragma region ScriptEngine Implementation

    void OnAppAssemblyFileSystemEvent(const std::string &path, const filewatch::Event event)
    {
        if (!m_ScriptEngineData->IsReloadingAssembly && event == filewatch::Event::modified)
        {
            m_ScriptEngineData->IsReloadingAssembly = true;

            Core::Application::Get().SubmitToMainThread([]()
                                                        {
				m_ScriptEngineData->AppAssemblyFileWatcher.reset();
				ScriptEngine::ReloadAssembly(); });
        }
    }

    void ScriptEngine::Init()
    {
        m_ScriptEngineData = new ScriptEngineData();

        m_ScriptEngineData->CoreAssemblyPath = "scripts/bin/Engine-ScriptCore.dll";
        m_ScriptEngineData->AppAssemblyPath = "scripts/bin/Sandbox.dll";

        InitMono();
        CreateDomainAndLoadAssembly();

        ScriptGlue::RegisterFuncs();
        ScriptGlue::RegisterComponents();

        m_ScriptEngineData->EntityClass = new ScriptClass("Engine", "Entity", true);
        ENGINE_ASSERT(m_ScriptEngineData->EntityClass);

        LoadAllAssemblyClasses();
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

    void ScriptEngine::ReloadAssembly()
    {
        ENGINE_INFO("ReloadAssembly");
        mono_domain_set(mono_get_root_domain(), false);

        mono_domain_unload(m_ScriptEngineData->AppDomain);

        CreateDomainAndLoadAssembly();

        m_ScriptEngineData->EntityClass = new ScriptClass("Engine", "Entity", true);
        ENGINE_ASSERT(m_ScriptEngineData->EntityClass);

        LoadAllAssemblyClasses();

        ScriptGlue::RegisterComponents();
    }

    void ScriptEngine::OnRuntimeStart(Scene *scene)
    {
        m_ScriptEngineData->SceneContext = scene;
    }
    Ref<ScriptInstance> ScriptEngine::GetEntityScriptInstance(UUID entityID)
    {
        auto it = m_ScriptEngineData->EntityInstances.find(entityID);
        if (it == m_ScriptEngineData->EntityInstances.end())
            return nullptr;

        return it->second;
    }
    Ref<ScriptClass> ScriptEngine::GetEntityClass(std::string className)
    {
        auto it = m_ScriptEngineData->EntityClasses.find(className);
        if (it == m_ScriptEngineData->EntityClasses.end())
            return nullptr;

        return it->second;
    }

    ScriptFieldMap &ScriptEngine::GetScriptFieldMap(UUID entityID)
    {
        auto it = m_ScriptEngineData->EntityFieldMaps.find(entityID);
        if (it == m_ScriptEngineData->EntityFieldMaps.end())
        {
            m_ScriptEngineData->EntityFieldMaps[entityID] = ScriptFieldMap();
            return m_ScriptEngineData->EntityFieldMaps[entityID];
        }
        return it->second;
    }
    MonoObject *ScriptEngine::GetManagedInstance(UUID entityID)
    {
        auto it = m_ScriptEngineData->EntityInstances.find(entityID);
        if (it == m_ScriptEngineData->EntityInstances.end())
            return nullptr;
        return it->second->GetManagedObject();
    }
    void ScriptEngine::OnCreateEntity(Entity entity)
    {
        std::string scriptClassName = entity.GetComponent<ScriptComponent>().ScriptClassName;
        if (EntityClassExists(scriptClassName))
        {
            ENGINE_INFO("Creating script instance for entity '{}' (id:{}) with class '{}'", entity.GetName(), (uint64_t)entity.GetUUID(), scriptClassName);
            Ref<ScriptClass> scriptClass = m_ScriptEngineData->EntityClasses[scriptClassName];
            Ref<ScriptInstance> instance = CreateRef<ScriptInstance>(scriptClass, entity);
            UUID entityID = entity.GetUUID();
            m_ScriptEngineData->EntityInstances[entityID] = instance;
            if (m_ScriptEngineData->EntityFieldMaps.find(entityID) != m_ScriptEngineData->EntityFieldMaps.end())
            {
                const ScriptFieldMap &fieldMap = m_ScriptEngineData->EntityFieldMaps.at(entityID);
                for (const auto &[name, fieldInstance] : fieldMap)
                    instance->SetFieldValueInternal(name, fieldInstance.m_Buffer);
            }

            instance->InvokeOnCreate();
        }
        else
        {
            ENGINE_ERROR("Script class '{}' not found for entity '{}'", scriptClassName, entity.GetName());
        }
    }

    void ScriptEngine::OnUpdateEntity(Entity entity, Core::Timestep ts)
    {
        UUID entityID = entity.GetComponent<IDComponent>().ID;
        if (m_ScriptEngineData->EntityInstances.find(entityID) == m_ScriptEngineData->EntityInstances.end())
        {
            ENGINE_WARN("No script instance found for entity ID {}", (uint64_t)entityID);
            return;
        }
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
    }

    void ScriptEngine::ShutdownMono()
    {
        mono_assemblies_cleanup();
        mono_domain_unload(m_ScriptEngineData->AppDomain);
        mono_domain_unload(m_ScriptEngineData->RootDomain);
        m_ScriptEngineData->AppDomain = nullptr;
        m_ScriptEngineData->RootDomain = nullptr;
    }

    void ScriptEngine::CreateDomainAndLoadAssembly()
    {
        char domainName[] = "EngineScriptRuntime";
        MonoDomain *appDomain = mono_domain_create_appdomain(domainName, nullptr);
        ENGINE_ASSERT(appDomain);
        mono_domain_set(appDomain, true);
        m_ScriptEngineData->AppDomain = appDomain;

        m_ScriptEngineData->CoreAssembly = LoadCSharpAssembly(m_ScriptEngineData->CoreAssemblyPath);
        m_ScriptEngineData->CoreAssemblyImage = mono_assembly_get_image(m_ScriptEngineData->CoreAssembly);
        ENGINE_ASSERT(m_ScriptEngineData->CoreAssemblyImage);
        PrintAssemblyTypes(m_ScriptEngineData->CoreAssembly);

        m_ScriptEngineData->AppAssembly = LoadCSharpAssembly(m_ScriptEngineData->AppAssemblyPath);
        m_ScriptEngineData->AppAssemblyImage = mono_assembly_get_image(m_ScriptEngineData->AppAssembly);
        ENGINE_ASSERT(m_ScriptEngineData->AppAssemblyImage);
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
            std::string fullClassName;
            if (nameSpace == nullptr || strlen(nameSpace) == 0)
                fullClassName = std::string(name);
            else
                fullClassName = std::string(nameSpace) + "." + std::string(name);

            MonoClass *monoClass = mono_class_from_name(image, nameSpace, name);
            ENGINE_INFO("fullClassName:{}", fullClassName);
            if (!monoClass)
            {
                continue;
            }
            if (fullClassName == "Engine.Entity")
                continue;
            if (mono_class_is_subclass_of(monoClass, m_ScriptEngineData->EntityClass->GetMonoClass(), false) == false)
                continue;
            Ref<ScriptClass> scriptClass = CreateRef<ScriptClass>(nameSpace, name);
            if (m_ScriptEngineData->EntityClasses.contains(fullClassName))
            {
                ENGINE_WARN("Already Loaded script class: {}", fullClassName);
            }
            m_ScriptEngineData->EntityClasses[fullClassName] = scriptClass;
            ENGINE_INFO("Loaded script class: {}", fullClassName);

            int fieldCount = mono_class_num_fields(monoClass);
            if (fieldCount > 0)
            {
                ENGINE_INFO("\t{} fields found for class {}", fieldCount, fullClassName);
                void *iter = nullptr;
                MonoClassField *field = nullptr;
                MonoObject *currentClassInstance = InitializeClass(m_ScriptEngineData->AppDomain, monoClass);
                uint8_t defaultValueBuffer[MaxScriptFieldBufferSize];
                while ((field = mono_class_get_fields(monoClass, &iter)) != nullptr)
                {
                    std::string fieldName = mono_field_get_name(field);
                    uint32_t fieldAccessibility = mono_field_get_flags(field);
                    ENGINE_INFO("\t\tField: {} (flags: {})", fieldName, fieldAccessibility);
                    if (fieldAccessibility & MONO_FIELD_ATTR_PUBLIC)
                    {
                        ScriptField scriptField;
                        scriptField.Name = fieldName;
                        scriptField.MonoField = field;
                        MonoType *fieldType = mono_field_get_type(field);
                        scriptField.FieldType = MonoTypeToScriptFieldType(fieldType, m_ScriptEngineData->EntityClass->GetMonoClass());
                        memset(defaultValueBuffer, 0, sizeof(defaultValueBuffer));
                        GetFieldDefaultValue(currentClassInstance, field, scriptField.FieldType, defaultValueBuffer);
                        memcpy(scriptField.DefaultValue, defaultValueBuffer, MaxScriptFieldBufferSize);
                        scriptClass->m_Fields[fieldName] = scriptField;
                    }
                }
            }
        }
    }
    void ScriptEngine::LoadAllAssemblyClasses()
    {
        m_ScriptEngineData->EntityClasses.clear();
        LoadAssemblyClasses(m_ScriptEngineData->CoreAssembly);
        LoadAssemblyClasses(m_ScriptEngineData->AppAssembly);

        m_ScriptEngineData->AppAssemblyFileWatcher = CreateScope<filewatch::FileWatch<std::string>>(
            m_ScriptEngineData->AppAssemblyPath.string(), OnAppAssemblyFileSystemEvent);
        m_ScriptEngineData->IsReloadingAssembly = false;
    }
#pragma endregion

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
    bool ScriptInstance::GetFieldValueInternal(const std::string &fieldName, void *value)
    {
        auto &fields = m_ScriptClass->GetFields();
        auto it = fields.find(fieldName);
        if (it == fields.end())
            return false;

        if (it->second.IsEntity())
        {
            uint64_t entityID = GetEntityIDFromEntityField(m_Instance, it->second.MonoField);
            memcpy(value, &entityID, sizeof(uint64_t));
            return true;
        }
        const ScriptField &scriptField = it->second;
        mono_field_get_value(m_Instance, scriptField.MonoField, value);
        return true;
    }

    bool ScriptInstance::SetFieldValueInternal(const std::string &fieldName, const void *value)
    {
        auto &fields = m_ScriptClass->GetFields();
        auto it = fields.find(fieldName);
        if (it == fields.end())
            return false;

        const ScriptField &scriptField = it->second;
        mono_field_set_value(m_Instance, scriptField.MonoField, (void *)value);
        return true;
    }

#pragma endregion
}
