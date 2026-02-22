#include "EntitySerialize.h"
#include "Engine/Scripts/ScriptEngine.h"
namespace Engine::Serialization
{
    void SerializeEntity(YAML::Emitter &out, Entity entity)
    {
        out << YAML::BeginMap; // Entity
        out << YAML::Key << "EntityID" << YAML::Value << entity.GetUUID();

        if (entity.HasComponent<TagComponent>())
        {
            auto &tagComp = entity.GetComponent<TagComponent>();
            SerializeTagComponent(out, tagComp);
        }
        if (entity.HasComponent<TransformComponent>())
        {
            auto &transformComp = entity.GetComponent<TransformComponent>();
            SerializeTransformComponent(out, transformComp);
        }
        if (entity.HasComponent<CameraComponent>())
        {
            auto &cameraComp = entity.GetComponent<CameraComponent>();
            SerializeCameraComponent(out, cameraComp);
        }
        if (entity.HasComponent<SpriteComponent>())
        {
            auto &spriteComp = entity.GetComponent<SpriteComponent>();
            SerializeSpriteComponent(out, spriteComp);
        }
        if (entity.HasComponent<CircleComponent>())
        {
            auto &circleComp = entity.GetComponent<CircleComponent>();
            SerializeCircleComponent(out, circleComp);
        }
        if (entity.HasComponent<Rigidbody2DComponent>())
        {
            auto &rigidbody2DComp = entity.GetComponent<Rigidbody2DComponent>();
            SerializeRigidbody2DComponent(out, rigidbody2DComp);
        }
        if (entity.HasComponent<BoxCollider2DComponent>())
        {
            auto &boxCollider2DComp = entity.GetComponent<BoxCollider2DComponent>();
            SerializeBoxCollider2DComponent(out, boxCollider2DComp);
        }
        if (entity.HasComponent<CircleCollider2DComponent>())
        {
            auto &circleCollider2DComp = entity.GetComponent<CircleCollider2DComponent>();
            SerializeCircleCollider2DComponent(out, circleCollider2DComp);
        }
        if (entity.HasComponent<ScriptComponent>())
        {
            auto &scriptComp = entity.GetComponent<ScriptComponent>();
            SerializeScriptComponent(out, scriptComp);
            // Serialize script fields
            UUID entityID = entity.GetUUID();
            auto &fieldMap = Engine::ScriptEngine::GetScriptFieldMap(entityID);
            if (!fieldMap.empty())
            {
                std::string scriptClassName = scriptComp.ScriptClassName;
                std::string fieldMapKey = scriptClassName + "_Fields";
                out << YAML::Key << fieldMapKey;
                out << YAML::BeginMap;
                for (const auto &[fieldName, fieldValue] : fieldMap)
                {
                    out << YAML::Key << fieldName;
                    out << YAML::BeginMap;
                    out << YAML::Key << "Name" << YAML::Value << fieldValue.Field.Name;
                    out << YAML::Key << "Type" << YAML::Value << (int)fieldValue.Field.FieldType;
                    if (fieldValue.Field.FieldType == ScriptFieldType::Int)
                    {
                        out << YAML::Key << "Value" << YAML::Value << fieldValue.GetValue<int>();
                    }
                    else if (fieldValue.Field.FieldType == ScriptFieldType::Float)
                    {
                        out << YAML::Key << "Value" << YAML::Value << fieldValue.GetValue<float>();
                    }
                    else if (fieldValue.Field.FieldType == ScriptFieldType::Bool)
                    {
                        out << YAML::Key << "Value" << YAML::Value << fieldValue.GetValue<bool>();
                    }
                    out << YAML::EndMap; // Field
                }
                out << YAML::EndMap; // Fields
            }
        }
        out << YAML::EndMap; // Entity
    }

    bool DeserializeEntity(const YAML::Node &entityNode, Entity &entity)
    {
        // 反序列化组件
        if (entityNode["TagComponent"])
        {
            auto &tagNode = entityNode["TagComponent"];
            auto &tagComp = entity.AddComponent<TagComponent>();
            if (!DeserializeTagComponent(tagNode, tagComp))
            {
                return false;
            }
        }
        if (entityNode["TransformComponent"])
        {
            auto &transformNode = entityNode["TransformComponent"];
            auto &transformComp = entity.AddComponent<TransformComponent>();
            if (!DeserializeTransformComponent(transformNode, transformComp))
            {
                return false;
            }
        }
        if (entityNode["CameraComponent"])
        {
            auto &cameraNode = entityNode["CameraComponent"];
            auto &cameraComp = entity.AddComponent<CameraComponent>();
            if (!DeserializeCameraComponent(cameraNode, cameraComp))
            {
                return false;
            }
        }
        if (entityNode["SpriteComponent"])
        {
            auto &spriteNode = entityNode["SpriteComponent"];
            auto &spriteComp = entity.AddComponent<SpriteComponent>();
            if (!DeserializeSpriteComponent(spriteNode, spriteComp))
            {
                return false;
            }
        }
        if (entityNode["CircleComponent"])
        {
            auto &circleNode = entityNode["CircleComponent"];
            auto &circleComp = entity.AddComponent<CircleComponent>();
            if (!DeserializeCircleComponent(circleNode, circleComp))
            {
                return false;
            }
        }
        if (entityNode["Rigidbody2DComponent"])
        {
            auto &rigidbody2DNode = entityNode["Rigidbody2DComponent"];
            auto &rigidbody2DComp = entity.AddComponent<Rigidbody2DComponent>();
            if (!DeserializeRigidbody2DComponent(rigidbody2DNode, rigidbody2DComp))
            {
                return false;
            }
        }
        if (entityNode["BoxCollider2DComponent"])
        {
            auto &boxCollider2DNode = entityNode["BoxCollider2DComponent"];
            auto &boxCollider2DComp = entity.AddComponent<BoxCollider2DComponent>();
            if (!DeserializeBoxCollider2DComponent(boxCollider2DNode, boxCollider2DComp))
            {
                return false;
            }
        }
        if (entityNode["CircleCollider2DComponent"])
        {
            auto circleCollider2DNode = entityNode["CircleCollider2DComponent"];
            auto &circleCollider2DComp = entity.AddComponent<CircleCollider2DComponent>();
            if (!DeserializeCircleCollider2DComponent(circleCollider2DNode, circleCollider2DComp))
            {
                return false;
            }
        }
        if (entityNode["ScriptComponent"])
        {
            auto &scriptNode = entityNode["ScriptComponent"];
            auto &scriptComp = entity.AddComponent<ScriptComponent>();
            if (!DeserializeScriptComponent(scriptNode, scriptComp))
            {
                return false;
            }
            std::string scriptClassName = scriptComp.ScriptClassName;
            std::string fieldMapKey = scriptClassName + "_Fields";
            if (entityNode[fieldMapKey])
            {
                UUID entityID = entity.GetUUID();
                auto &fieldMap = Engine::ScriptEngine::GetScriptFieldMap(entityID);
                for (auto &fieldNode : entityNode[fieldMapKey])
                {
                    std::string fieldName = fieldNode.first.as<std::string>();
                    auto &fieldDataNode = fieldNode.second;
                    ScriptFieldInstance fieldInstance;
                    fieldInstance.Field.Name = fieldDataNode["Name"].as<std::string>();
                    fieldInstance.Field.FieldType = (ScriptFieldType)fieldDataNode["Type"].as<int>();
                    if (fieldInstance.Field.FieldType == ScriptFieldType::Int)
                    {
                        int value = fieldDataNode["Value"].as<int>();
                        fieldInstance.SetValue<int>(value);
                    }
                    else if (fieldInstance.Field.FieldType == ScriptFieldType::Float)
                    {
                        float value = fieldDataNode["Value"].as<float>();
                        ENGINE_INFO("Deserialized field '{}' with value {}", fieldName, value);
                        fieldInstance.SetValue<float>(value);
                    }
                    else if (fieldInstance.Field.FieldType == ScriptFieldType::Bool)
                    {
                        bool value = fieldDataNode["Value"].as<bool>();
                        fieldInstance.SetValue<bool>(value);
                    }
                    fieldMap[fieldName] = fieldInstance;
                }
            }
        }
        return true;
    }
} // namespace Engine::Serialization