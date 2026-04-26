#include "ComponentSerialize.h"
#include "Engine/Core/Core.h"
#include "Engine/Scripts/ScriptEngine.h"
#include "Engine/Project/Project.h"
#include "Engine/Reflection/Reflect.h"

#define SERIALIZE_COMPONENT(out, obj, ComponentType)           \
    do                                                         \
    {                                                          \
        out << YAML::Key << #ComponentType;                    \
        SerializeValue(out, &(obj), Reflect<ComponentType>()); \
    } while (0)

#define DESERILIZE_COMPONENT(node, obj, ComponentType)              \
    do                                                              \
    {                                                               \
        DeserializeValue(&(obj), Reflect<ComponentType>(), (node)); \
    } while (0)
namespace Engine::Serialization
{
#pragma region Utils

    inline const char *EnumToString(const MetaType &type, int64_t value)
    {
        if (!type.enumValues)
            return nullptr;

        for (const auto &item : *type.enumValues)
        {
            if (item.Value == value)
                return item.Name;
        }
        return nullptr;
    }

    inline void SerializeValue(YAML::Emitter &out, const void *obj, const MetaType &type)
    {
        switch (type.kind)
        {
        case MetaKind::Bool:
            out << *static_cast<const bool *>(obj);
            break;

        case MetaKind::Int:
            out << *static_cast<const int *>(obj);
            break;

        case MetaKind::Float:
            out << *static_cast<const float *>(obj);
            break;

        case MetaKind::Float2:
            out << *static_cast<const glm::vec2 *>(obj);
            break;

        case MetaKind::Float3:
            out << *static_cast<const glm::vec3 *>(obj);
            break;

        case MetaKind::Float4:
            out << *static_cast<const glm::vec4 *>(obj);
            break;

        case MetaKind::String:
            out << *static_cast<const std::string *>(obj);
            break;

        case MetaKind::Enum:
        {
            int64_t value = type.EnumToInt(obj);
            const char *name = EnumToString(type, value);
            if (name)
                out << name;
            else
                out << value; // 找不到时降级写整数

            break;
        }

        case MetaKind::Struct:
        {
            out << YAML::BeginMap;

            if (type.fields)
            {
                for (const MetaField &field : *type.fields)
                {
                    out << YAML::Key << field.name;
                    out << YAML::Value;
                    SerializeValue(out, field.getConst(obj), *field.type);
                }
            }

            out << YAML::EndMap;
            break;
        }

        default:
            ENGINE_ERROR("Unsupported MetaKind in SerializeValue: {}", (int)type.kind);
            break;
        }
    }

    inline void DeserializeValue(void *obj, const MetaType &type, const YAML::Node &node)
    {
        if (!node)
            return;

        switch (type.kind)
        {
        case MetaKind::Bool:
            *static_cast<bool *>(obj) = node.as<bool>();
            break;

        case MetaKind::Int:
            *static_cast<int *>(obj) = node.as<int>();
            break;

        case MetaKind::Float:
            *static_cast<float *>(obj) = node.as<float>();
            break;

        case MetaKind::Float2:
            *static_cast<glm::vec2 *>(obj) = node.as<glm::vec2>();
            break;

        case MetaKind::Float3:
            *static_cast<glm::vec3 *>(obj) = node.as<glm::vec3>();
            break;

        case MetaKind::Float4:
            *static_cast<glm::vec4 *>(obj) = node.as<glm::vec4>();
            break;

        case MetaKind::String:
            *static_cast<std::string *>(obj) = node.as<std::string>();
            break;

        case MetaKind::Enum:
        {
            if (!type.enumValues || !type.EnumFromInt)
                return;
            int64_t value;
            if (node.IsScalar())
            {
                // 先尝试按名字解析
                std::string name = node.as<std::string>();
                value = -1;
                for (const auto &item : *type.enumValues)
                {
                    if (item.Name == name)
                    {
                        value = item.Value;
                        break;
                    }
                }
                if (value == -1)
                {
                    // 降级尝试按整数解析
                    value = node.as<int64_t>();
                }
            }
            else
            {
                value = node.as<int64_t>();
            }
            type.EnumFromInt(obj, value);
            break;
        }

        case MetaKind::Struct:
        {
            if (!node.IsMap() || !type.fields)
                return;

            for (const MetaField &field : *type.fields)
            {
                YAML::Node child = node[field.name];
                if (!child)
                    continue; // 缺字段就保留默认值

                DeserializeValue(field.get(obj), *field.type, child);
            }
            break;
        }

        default:
            throw std::runtime_error("Unsupported MetaKind in DeserializeValue");
        }
    }

#pragma endregion
    void SerializeTagComponent(YAML::Emitter &out, TagComponent &component)
    {
        SERIALIZE_COMPONENT(out, component, TagComponent);
    }

    void SerializeTransformComponent(YAML::Emitter &out, TransformComponent &component)
    {
        SERIALIZE_COMPONENT(out, component, TransformComponent);
    }

    void SerializeCameraComponent(YAML::Emitter &out, CameraComponent &component)
    {
        SERIALIZE_COMPONENT(out, component, CameraComponent);
    }

    void SerializeSpriteComponent(YAML::Emitter &out, SpriteComponent &component)
    {
        SERIALIZE_COMPONENT(out, component, SpriteComponent);
    }

    void SerializeCircleComponent(YAML::Emitter &out, CircleComponent &component)
    {
        SERIALIZE_COMPONENT(out, component, CircleComponent);
    }

    void SerializeRigidbody2DComponent(YAML::Emitter &out, Rigidbody2DComponent &component)
    {
        SERIALIZE_COMPONENT(out, component, Rigidbody2DComponent);
    }

    void SerializeBoxCollider2DComponent(YAML::Emitter &out, BoxCollider2DComponent &component)
    {
        SERIALIZE_COMPONENT(out, component, BoxCollider2DComponent);
    }

    void SerializeCircleCollider2DComponent(YAML::Emitter &out, CircleCollider2DComponent &component)
    {
        SERIALIZE_COMPONENT(out, component, CircleCollider2DComponent);
    }

    void SerializeScriptComponent(YAML::Emitter &out, ScriptComponent &component)
    {
        SERIALIZE_COMPONENT(out, component, ScriptComponent);
    }

    bool DeserializeTagComponent(const YAML::Node &componentNode, TagComponent &component)
    {
        DESERILIZE_COMPONENT(componentNode, component, TagComponent);
        return true;
    }

    bool DeserializeTransformComponent(const YAML::Node &componentNode, TransformComponent &component)
    {
        DESERILIZE_COMPONENT(componentNode, component, TransformComponent);
        return true;
    }

    bool DeserializeCameraComponent(const YAML::Node &componentNode, CameraComponent &component)
    {
        DESERILIZE_COMPONENT(componentNode, component, CameraComponent);
        return true;
    }

    bool DeserializeSpriteComponent(const YAML::Node &componentNode, SpriteComponent &component)
    {
        if (componentNode["Color"])
        {
            component.Color = componentNode["Color"].as<glm::vec4>();
        }
        else
        {
            LOG_WARN("SpriteComponent missing Color during deserialization. Setting to default white color.");
            component.Color = glm::vec4(1.0f);
        }
        if (componentNode["TexturePath"])
        {
            std::string texturePath = componentNode["TexturePath"].as<std::string>();
            if (texturePath != "None")
            {
                std::filesystem::path fullTexturePath = Engine::Project::GetActiveProjectAssetPath(texturePath);
                component.Texture = Engine::Renderer::Texture2D::Create(fullTexturePath.string());
                ENGINE_INFO("Loaded texture from path: {}", fullTexturePath.string());
            }
            else
            {
                component.Texture = nullptr;
            }
        }
        if (componentNode["TilingFactor"])
        {
            component.TilingFactor = componentNode["TilingFactor"].as<float>();
        }
        return true;
    }

    bool DeserializeCircleComponent(const YAML::Node &componentNode, CircleComponent &component)
    {
        DESERILIZE_COMPONENT(componentNode, component, CircleComponent);
        return true;
    }

    bool DeserializeRigidbody2DComponent(const YAML::Node &componentNode, Rigidbody2DComponent &component)
    {
        DESERILIZE_COMPONENT(componentNode, component, Rigidbody2DComponent);
        return true;
    }

    bool DeserializeBoxCollider2DComponent(const YAML::Node &componentNode, BoxCollider2DComponent &component)
    {
        DESERILIZE_COMPONENT(componentNode, component, BoxCollider2DComponent);
        return true;
    }

    bool DeserializeCircleCollider2DComponent(const YAML::Node &componentNode, CircleCollider2DComponent &component)
    {
        DESERILIZE_COMPONENT(componentNode, component, CircleCollider2DComponent);
        return true;
    }
    bool DeserializeScriptComponent(const YAML::Node &componentNode, ScriptComponent &component)
    {
        DESERILIZE_COMPONENT(componentNode, component, ScriptComponent);
        return true;
    }
} // namespace Engine::Serialization
