#include "EntitySerialize.h"
#include "Engine/Scripts/ScriptEngine.h"
#include <algorithm>
#include <vector>

#define SERIALIZE_SCRIPT_FIELD(fieldType, type)            \
    case fieldType:                                        \
    {                                                      \
        out << YAML::Key << "Value";                       \
        out << YAML::Value << fieldValue.GetValue<type>(); \
        break;                                             \
    }

#define DESERIALIZE_SCRIPT_FIELD(fieldType, type)                                                 \
    case fieldType:                                                                               \
    {                                                                                             \
        if (fieldDataNode["Value"])                                                               \
        {                                                                                         \
            fieldInstance.SetValue<type>(fieldDataNode["Value"].as<type>());                      \
        }                                                                                         \
        else                                                                                      \
        {                                                                                         \
            ENGINE_WARN("Failed to deserialize script field of type " + std::string(#fieldType)); \
        }                                                                                         \
        break;                                                                                    \
    }

namespace Engine::Serialization
{
    static void SerializeScriptFields(YAML::Emitter &out, Entity entity, const ScriptComponent &scriptComp)
    {
        UUID entityID = entity.GetUUID();
        auto &fieldMap = Engine::ScriptEngine::GetScriptFieldMap(entityID);
        if (fieldMap.empty())
        {
            return;
        }

        std::string fieldMapKey = scriptComp.ScriptClassName + "_Fields";
        out << YAML::Key << fieldMapKey;
        out << YAML::BeginMap;

        std::vector<std::string> fieldNames;
        fieldNames.reserve(fieldMap.size());
        for (const auto &[fieldName, fieldValue] : fieldMap)
        {
            fieldNames.push_back(fieldName);
        }
        std::sort(fieldNames.begin(), fieldNames.end());

        for (const auto &fieldName : fieldNames)
        {
            const auto &fieldValue = fieldMap.at(fieldName);
            out << YAML::Key << fieldName;
            out << YAML::BeginMap;
            out << YAML::Key << "Name" << YAML::Value << fieldValue.Field.Name;
            out << YAML::Key << "Type" << YAML::Value << (int)fieldValue.Field.FieldType;
            switch (fieldValue.Field.FieldType)
            {
                SERIALIZE_SCRIPT_FIELD(ScriptFieldType::Float, float);
                SERIALIZE_SCRIPT_FIELD(ScriptFieldType::Double, double);
                SERIALIZE_SCRIPT_FIELD(ScriptFieldType::Bool, bool);
                SERIALIZE_SCRIPT_FIELD(ScriptFieldType::Char, char);
                SERIALIZE_SCRIPT_FIELD(ScriptFieldType::UByte, uint8_t);
                SERIALIZE_SCRIPT_FIELD(ScriptFieldType::Byte, int8_t);
                SERIALIZE_SCRIPT_FIELD(ScriptFieldType::Short, int16_t);
                SERIALIZE_SCRIPT_FIELD(ScriptFieldType::UShort, uint16_t);
                SERIALIZE_SCRIPT_FIELD(ScriptFieldType::Int, int);
                SERIALIZE_SCRIPT_FIELD(ScriptFieldType::UInt, uint32_t);
                SERIALIZE_SCRIPT_FIELD(ScriptFieldType::Long, int64_t);
                SERIALIZE_SCRIPT_FIELD(ScriptFieldType::ULong, uint64_t);
                SERIALIZE_SCRIPT_FIELD(ScriptFieldType::Vector2, glm::vec2);
                SERIALIZE_SCRIPT_FIELD(ScriptFieldType::Vector3, glm::vec3);
                SERIALIZE_SCRIPT_FIELD(ScriptFieldType::Vector4, glm::vec4);
                SERIALIZE_SCRIPT_FIELD(ScriptFieldType::Entity, UUID);
                SERIALIZE_SCRIPT_FIELD(ScriptFieldType::Component, UUID);
            }
            out << YAML::EndMap; // Field
        }

        out << YAML::EndMap; // Fields
    }

    static void DeserializeScriptFields(const YAML::Node &entityNode, Entity &entity, const ScriptComponent &scriptComp)
    {
        std::string fieldMapKey = scriptComp.ScriptClassName + "_Fields";
        if (!entityNode[fieldMapKey])
        {
            return;
        }

        UUID entityID = entity.GetUUID();
        auto &fieldMap = Engine::ScriptEngine::GetScriptFieldMap(entityID);
        auto scriptClass = Engine::ScriptEngine::GetEntityClass(scriptComp.ScriptClassName);
        const auto *scriptFields = scriptClass ? &scriptClass->GetFields() : nullptr;

        if (!scriptFields)
        {
            ENGINE_WARN("Cannot validate serialized fields for missing script class '{}'", scriptComp.ScriptClassName);
        }

        for (auto &fieldNode : entityNode[fieldMapKey])
        {
            std::string fieldName = fieldNode.first.as<std::string>();
            auto &fieldDataNode = fieldNode.second;
            ScriptFieldInstance fieldInstance;

            if (scriptFields)
            {
                auto currentFieldIt = scriptFields->find(fieldName);
                if (currentFieldIt == scriptFields->end())
                {
                    ENGINE_WARN("Skipping serialized field '{}' because it no longer exists on script '{}'", fieldName, scriptComp.ScriptClassName);
                    continue;
                }

                fieldInstance.Field = currentFieldIt->second;
                fieldInstance.CopyValueToBuffer(fieldInstance.Field.DefaultValue, sizeof(fieldInstance.Field.DefaultValue));

                if (fieldDataNode["Type"])
                {
                    auto serializedType = (ScriptFieldType)fieldDataNode["Type"].as<int>();
                    if (serializedType != fieldInstance.Field.FieldType)
                    {
                        ENGINE_WARN("Skipping serialized field '{}' on script '{}' because its type changed", fieldName, scriptComp.ScriptClassName);
                        continue;
                    }
                }
            }
            else
            {
                if (!fieldDataNode["Name"] || !fieldDataNode["Type"])
                {
                    ENGINE_WARN("Skipping malformed serialized field '{}' on missing script '{}'", fieldName, scriptComp.ScriptClassName);
                    continue;
                }

                fieldInstance.Field.Name = fieldDataNode["Name"].as<std::string>();
                fieldInstance.Field.FieldType = (ScriptFieldType)fieldDataNode["Type"].as<int>();
            }

            switch (fieldInstance.Field.FieldType)
            {
                DESERIALIZE_SCRIPT_FIELD(ScriptFieldType::Float, float);
                DESERIALIZE_SCRIPT_FIELD(ScriptFieldType::Double, double);
                DESERIALIZE_SCRIPT_FIELD(ScriptFieldType::Bool, bool);
                DESERIALIZE_SCRIPT_FIELD(ScriptFieldType::Char, char);
                DESERIALIZE_SCRIPT_FIELD(ScriptFieldType::UByte, uint8_t);
                DESERIALIZE_SCRIPT_FIELD(ScriptFieldType::Byte, int8_t);
                DESERIALIZE_SCRIPT_FIELD(ScriptFieldType::Short, int16_t);
                DESERIALIZE_SCRIPT_FIELD(ScriptFieldType::UShort, uint16_t);
                DESERIALIZE_SCRIPT_FIELD(ScriptFieldType::Int, int);
                DESERIALIZE_SCRIPT_FIELD(ScriptFieldType::UInt, uint32_t);
                DESERIALIZE_SCRIPT_FIELD(ScriptFieldType::Long, int64_t);
                DESERIALIZE_SCRIPT_FIELD(ScriptFieldType::ULong, uint64_t);
                DESERIALIZE_SCRIPT_FIELD(ScriptFieldType::Vector2, glm::vec2);
                DESERIALIZE_SCRIPT_FIELD(ScriptFieldType::Vector3, glm::vec3);
                DESERIALIZE_SCRIPT_FIELD(ScriptFieldType::Vector4, glm::vec4);
                DESERIALIZE_SCRIPT_FIELD(ScriptFieldType::Entity, UUID);
                DESERIALIZE_SCRIPT_FIELD(ScriptFieldType::Component, UUID);
            default:
            {
                ENGINE_WARN("Unknown script field type for field " + fieldName);
                break;
            }
            }
            fieldMap[fieldName] = fieldInstance;
        }
    }
#pragma region Component Serialization
    template <IsSerializableComponent T>
    static void SerializeComponentIfExists(YAML::Emitter &out, Entity entity)
    {
        if (!entity.HasComponent<T>())
        {
            return;
        }

        auto &component = entity.GetComponent<T>();
        SerializeComponent(out, component);
    }

    template <>
    void SerializeComponentIfExists<ScriptComponent>(YAML::Emitter &out, Entity entity)
    {
        if (!entity.HasComponent<ScriptComponent>())
        {
            return;
        }

        auto &scriptComp = entity.GetComponent<ScriptComponent>();
        SerializeComponent(out, scriptComp);
        SerializeScriptFields(out, entity, scriptComp);
    }

    template <typename... T>
    static void SerializeComponents(YAML::Emitter &out, Entity entity, ComponentGroup<T...>)
    {
        (SerializeComponentIfExists<T>(out, entity), ...);
    }

    template <IsSerializableComponent T>
    static bool DeserializeComponentIfExists(const YAML::Node &entityNode, Entity &entity)
    {
        const char *componentName = Reflect<T>().name;
        if (!entityNode[componentName])
        {
            return true;
        }

        auto &componentNode = entityNode[componentName];
        auto &component = entity.AddComponent<T>();
        return DeserializeComponent(componentNode, component);
    }

    template <>
    bool DeserializeComponentIfExists<ScriptComponent>(const YAML::Node &entityNode, Entity &entity)
    {
        const char *componentName = Reflect<ScriptComponent>().name;
        if (!entityNode[componentName])
        {
            return true;
        }
        auto &componentNode = entityNode[componentName];
        auto &scriptComp = entity.AddComponent<ScriptComponent>();
        if (!DeserializeComponent(componentNode, scriptComp))
        {
            return false;
        }
        DeserializeScriptFields(entityNode, entity, scriptComp);
        return true;
    }

    template <typename... T>
    static bool DeserializeComponents(const YAML::Node &entityNode, Entity &entity, ComponentGroup<T...>)
    {
        return (DeserializeComponentIfExists<T>(entityNode, entity) && ...);
    }

#pragma endregion

    void SerializeEntity(YAML::Emitter &out, Entity entity)
    {
        out << YAML::BeginMap; // Entity
        out << YAML::Key << "EntityID" << YAML::Value << entity.GetUUID();

        SerializeComponents(out, entity, REFLECT_SERIALIZE_TYPE{});

        out << YAML::EndMap; // Entity
    }

    bool DeserializeEntity(const YAML::Node &entityNode, Entity &entity)
    {
        if (!DeserializeComponents(entityNode, entity, REFLECT_SERIALIZE_TYPE{}))
        {
            return false;
        }
        return true;
    }
} // namespace Engine::Serialization
