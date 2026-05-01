#pragma once
#include "yaml-cpp/yaml.h"
#include "Engine/Scene/Component.h"
#include "SerializeUtils.h"
#include <type_traits>

#define SERIALIZE_COMPONENT(out, obj, ComponentType)           \
    do                                                         \
    {                                                          \
        out << YAML::Key << Reflect<ComponentType>().name;     \
        SerializeValue(out, &(obj), Reflect<ComponentType>()); \
    } while (0)

#define DESERILIZE_COMPONENT(node, obj, ComponentType)              \
    do                                                              \
    {                                                               \
        DeserializeValue(&(obj), Reflect<ComponentType>(), (node)); \
    } while (0)

namespace Engine::Serialization
{
    using REFLECT_SERIALIZE_TYPE = ComponentGroup<
        TagComponent,
        TransformComponent,
        CameraComponent,
        SpriteComponent,
        CircleComponent,
        Rigidbody2DComponent,
        BoxCollider2DComponent,
        CircleCollider2DComponent,
        ScriptComponent>;

    template <typename T>
    concept IsSerializableComponent = IsInComponentGroup<T, REFLECT_SERIALIZE_TYPE>;

    template <typename T>
    void SerializeComponentImpl(YAML::Emitter &out, const T &component);

    void SerializeComponentImpl(YAML::Emitter &out, const SpriteComponent &component);

    template <typename T>
    bool DeserializeComponentImpl(const YAML::Node &componentNode, T &component);

    bool DeserializeComponentImpl(const YAML::Node &componentNode, SpriteComponent &component);

    template <IsSerializableComponent T>
    void SerializeComponent(YAML::Emitter &out, const T &component)
    {
        SerializeComponentImpl(out, component);
    }

    template <IsSerializableComponent T>
    bool DeserializeComponent(const YAML::Node &componentNode, T &component)
    {
        return DeserializeComponentImpl(componentNode, component);
    }

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
                    if (!HasPropertyFlag(field.flags, PropertyFlags::Property_Serializable))
                        continue; // 不参与序列化
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

    template <typename T>
    void SerializeComponentImpl(YAML::Emitter &out, const T &component)
    {
        using ComponentType = std::decay_t<decltype(component)>;
        SERIALIZE_COMPONENT(out, component, ComponentType);
    }

    template <typename T>
    bool DeserializeComponentImpl(const YAML::Node &componentNode, T &component)
    {
        using ComponentType = std::decay_t<decltype(component)>;
        DESERILIZE_COMPONENT(componentNode, component, ComponentType);
        return true;
    }

} // namespace Engine::Serialization