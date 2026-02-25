#pragma once
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Scene.h"
extern "C"
{
    typedef struct _MonoDomain MonoDomain;
    typedef struct _MonoAssembly MonoAssembly;
    typedef struct _MonoImage MonoImage;
    typedef struct _MonoObject MonoObject;
    typedef struct _MonoClass MonoClass;
    typedef struct _MonoMethod MonoMethod;
    typedef struct _MonoClassField MonoClassField;
}
namespace Engine
{
    const size_t MaxScriptFieldBufferSize = 16;
    enum class ScriptFieldType
    {
        None = 0,
        Float,
        Double,
        Bool,
        Char,
        UByte,
        Byte,
        Short,
        UShort,
        Int,
        UInt,
        Long,
        ULong,
        Vector2,
        Vector3,
        Vector4,
        Entity
    };
    struct ScriptField
    {
        std::string Name;
        ScriptFieldType FieldType;
        MonoClassField *MonoField;
        uint8_t DefaultValue[MaxScriptFieldBufferSize];

        template <typename T>
        T GetDefaultValue() const
        {
            static_assert(sizeof(T) <= MaxScriptFieldBufferSize, "Type too large!");
            return *(T *)DefaultValue;
        }
        template <typename T>
        void SetDefaultValue(const T &value)
        {
            static_assert(sizeof(T) <= MaxScriptFieldBufferSize, "Type too large!");
            memcpy(DefaultValue, &value, sizeof(T));
        }

        bool IsEntity() const
        {
            return FieldType == ScriptFieldType::Entity;
        }
    };
    class ScriptInstance;
    class ScriptClass;
    struct ScriptFieldInstance;
    using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;
    class ScriptEngine
    {
    public:
        static void Init();
        static void Shutdown();

        static Scene *GetSceneContext();
        static MonoImage *GetCoreAssemblyImage();

        static Ref<ScriptInstance> GetEntityScriptInstance(UUID entityID);
        static Ref<ScriptClass> GetEntityClass(std::string className);
        static ScriptFieldMap &GetScriptFieldMap(UUID entityID);

        static MonoObject *GetManagedInstance(UUID entityID);

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
        ScriptClass(const std::string &namespaceName, const std::string &className, bool isCore = false);
        ~ScriptClass();

        std::string GetFullClassName() { return m_Namespace + "." + m_ClassName; }

        MonoObject *Instantiate();
        MonoClass *GetMonoClass() { return m_MonoClass; }
        MonoMethod *GetMethod(const std::string &methodName, int paramCount);
        void InvokeMethod(MonoObject *instance, const std::string &methodName, int paramCount = 0, void **params = nullptr);
        void InvokeMethod(MonoObject *instance, MonoMethod *method, int paramCount = 0, void **params = nullptr);

        const std::map<std::string, ScriptField> &GetFields() const { return m_Fields; }

    private:
        friend class ScriptEngine;
        std::string m_Namespace;
        std::string m_ClassName;
        MonoClass *m_MonoClass = nullptr;

        std::map<std::string, ScriptField> m_Fields;
    };

    struct ScriptFieldInstance
    {
        ScriptField Field;

        ScriptFieldInstance()
        {
            memset(m_Buffer, 0, sizeof(m_Buffer));
        }

        template <typename T>
        T GetValue() const
        {
            static_assert(sizeof(T) <= MaxScriptFieldBufferSize, "Type too large!");
            return *(T *)m_Buffer;
        }

        template <typename T>
        void SetValue(T value)
        {
            static_assert(sizeof(T) <= MaxScriptFieldBufferSize, "Type too large!");
            memcpy(m_Buffer, &value, sizeof(T));
        }

        void CopyValueToBuffer(const uint8_t *value, size_t size)
        {
            memset(m_Buffer, 0, sizeof(m_Buffer));
            ENGINE_ASSERT(size <= MaxScriptFieldBufferSize, "Type too large!");
            // ENGINE_INFO("Copying default value to buffer for field '{}' with size {}", Field.Name, size);
            // ENGINE_INFO("Default value for field '{}' is {}", Field.Name, *(int32_t *)value);
            memcpy(m_Buffer, value, size);
            // ENGINE_INFO("Buffer value for field '{}' is now {}", Field.Name, *(int32_t *)m_Buffer);
        }

    private:
        uint8_t m_Buffer[MaxScriptFieldBufferSize];

        friend class ScriptEngine;
        friend class ScriptInstance;
    };

    /// @brief Represents an instance of a script class attached to an entity.
    class ScriptInstance
    {
    public:
        ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);
        ~ScriptInstance() = default;

        void InvokeOnCreate();
        void InvokeOnUpdate(float deltaTime);

        Ref<ScriptClass> GetScriptClass() const { return m_ScriptClass; }

        template <typename T>
        T GetFieldValue(const std::string &fieldName)
        {
            if (GetFieldValueInternal(fieldName, s_FieldValueBuffer) == false)
            {
                ENGINE_WARN("Failed to get field value for field '{}'!", fieldName);
                return T();
            }
            return *(T *)s_FieldValueBuffer;
        }

        template <typename T>
        void SetFieldValue(const std::string &fieldName, const T &value)
        {
            SetFieldValueInternal(fieldName, &value);
        }

        MonoObject *GetManagedObject() const { return m_Instance; }

    private:
        bool GetFieldValueInternal(const std::string &fieldName, void *value);
        bool SetFieldValueInternal(const std::string &fieldName, const void *value);

    private:
        Ref<ScriptClass> m_ScriptClass;
        MonoObject *m_Instance = nullptr;

        MonoMethod *m_Constructor = nullptr;
        MonoMethod *m_OnCreateMethod = nullptr;
        MonoMethod *m_OnUpdateMethod = nullptr;

        inline static char s_FieldValueBuffer[MaxScriptFieldBufferSize];

        friend class ScriptEngine;
        friend struct ScriptFieldInstance;
    };
}