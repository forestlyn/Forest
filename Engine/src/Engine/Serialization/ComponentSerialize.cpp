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
            throw std::runtime_error("Unsupported MetaKind in SerializeValue");
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
        out << YAML::Key << "CameraComponent";
        out << YAML::BeginMap;
        out << YAML::Key << "Primary" << YAML::Value << component.Primary;
        out << YAML::Key << "FixedAspectRatio" << YAML::Value << component.FixedAspectRatio;

        out << YAML::Key << "ProjectionType" << YAML::Value << (int)component.ProjectionType;
        out << YAML::Key << "AspectRatio" << YAML::Value << component.AspectRatio;
        // Orthographic properties
        out << YAML::Key << "OrthographicSize" << YAML::Value << component.OrthographicSize;
        out << YAML::Key << "OrthographicNearClip" << YAML::Value << component.OrthographicNear;
        out << YAML::Key << "OrthographicFarClip" << YAML::Value << component.OrthographicFar;
        // Perspective properties
        out << YAML::Key << "PerspectiveFOV" << YAML::Value << component.PerspectiveFOV;
        out << YAML::Key << "PerspectiveNearClip" << YAML::Value << component.PerspectiveNear;
        out << YAML::Key << "PerspectiveFarClip" << YAML::Value << component.PerspectiveFar;

        out << YAML::EndMap;
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
        out << YAML::Key << "Rigidbody2DComponent";
        out << YAML::BeginMap;
        out << YAML::Key << "Type" << YAML::Value << (int)component.Type;
        out << YAML::Key << "Velocity" << YAML::Value << component.Velocity;
        out << YAML::Key << "AngularVelocity" << YAML::Value << component.AngularVelocity;
        out << YAML::Key << "FixedRotation" << YAML::Value << component.FixedRotation;
        out << YAML::EndMap;
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
        if (componentNode["Primary"])
        {
            component.Primary = componentNode["Primary"].as<bool>();
        }
        if (componentNode["FixedAspectRatio"])
        {
            component.FixedAspectRatio = componentNode["FixedAspectRatio"].as<bool>();
        }
        if (componentNode["ProjectionType"])
        {
            component.ProjectionType = (SceneCameraProjectionType)componentNode["ProjectionType"].as<int>();
        }
        if (componentNode["AspectRatio"])
        {
            component.AspectRatio = componentNode["AspectRatio"].as<float>();
        }
        if (componentNode["OrthographicSize"])
        {
            component.OrthographicSize = componentNode["OrthographicSize"].as<float>();
        }
        if (componentNode["OrthographicNearClip"])
        {
            component.OrthographicNear = componentNode["OrthographicNearClip"].as<float>();
        }
        if (componentNode["OrthographicFarClip"])
        {
            component.OrthographicFar = componentNode["OrthographicFarClip"].as<float>();
        }
        if (componentNode["PerspectiveFOV"])
        {
            component.PerspectiveFOV = componentNode["PerspectiveFOV"].as<float>();
        }
        if (componentNode["PerspectiveNearClip"])
        {
            component.PerspectiveNear = componentNode["PerspectiveNearClip"].as<float>();
        }
        if (componentNode["PerspectiveFarClip"])
        {
            component.PerspectiveFar = componentNode["PerspectiveFarClip"].as<float>();
        }
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
        if (componentNode["Type"])
        {
            component.Type = (Rigidbody2DComponent::BodyType)componentNode["Type"].as<int>();
        }
        if (componentNode["Velocity"])
        {
            component.Velocity = componentNode["Velocity"].as<glm::vec2>();
        }
        if (componentNode["AngularVelocity"])
        {
            component.AngularVelocity = componentNode["AngularVelocity"].as<float>();
        }
        if (componentNode["FixedRotation"])
        {
            component.FixedRotation = componentNode["FixedRotation"].as<bool>();
        }
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
