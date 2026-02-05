#pragma once
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Scene.h"
extern "C"
{
    typedef struct _MonoObject MonoObject;
    typedef struct _MonoClass MonoClass;
    typedef struct _MonoMethod MonoMethod;
    typedef struct _MonoDomain MonoDomain;
    typedef struct _MonoAssembly MonoAssembly;
    typedef struct _MonoImage MonoImage;
}
namespace Engine
{
    class ScriptEngine
    {
    public:
        static void Init();
        static void Shutdown();

        static Scene *GetSceneContext();
        static MonoImage *GetCoreAssemblyImage();

        static void OnRuntimeStart(Scene *scene);
        static void OnCreateEntity(Entity entity);

        static void OnUpdateEntity(Entity entity, Core::Timestep ts);

        static bool EntityClassExists(const std::string &className);

    private:
        static void InitMono();
        static void ShutdownMono();

        static void LoadAssemblyClasses(MonoAssembly *assembly);
    };

    class ScriptClass
    {
    public:
        ScriptClass() = default;
        ScriptClass(const std::string &namespaceName, const std::string &className);
        ~ScriptClass();

        MonoObject *Instantiate();
        MonoClass *GetMonoClass() { return m_MonoClass; }
        MonoMethod *GetMethod(const std::string &methodName, int paramCount);
        void InvokeMethod(MonoObject *instance, const std::string &methodName, int paramCount = 0, void **params = nullptr);
        void InvokeMethod(MonoObject *instance, MonoMethod *method, int paramCount = 0, void **params = nullptr);

    private:
        std::string m_Namespace;
        std::string m_ClassName;
        MonoClass *m_MonoClass = nullptr;
    };

    class ScriptInstance
    {
    public:
        ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);
        ~ScriptInstance() = default;

        void InvokeOnCreate();
        void InvokeOnUpdate(float deltaTime);

    private:
        Ref<ScriptClass> m_ScriptClass;
        MonoObject *m_Instance = nullptr;

        MonoMethod *m_Constructor = nullptr;
        MonoMethod *m_OnCreateMethod = nullptr;
        MonoMethod *m_OnUpdateMethod = nullptr;
    };
}