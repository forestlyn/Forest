#include "ComponentSerialize.h"
#include "Engine/Core/Core.h"
namespace Engine::Serialization
{
    void SerilizaTagComponent(YAML::Emitter &out, TagComponent &entity)
    {
        out << YAML::Key << "TagComponent";
        out << YAML::BeginMap;
        out << YAML::Key << "Tag" << YAML::Value << entity.Tag;
        out << YAML::EndMap;
    }

    void SerilizaTransformComponent(YAML::Emitter &out, TransformComponent &component)
    {
        out << YAML::Key << "TransformComponent";
        out << YAML::BeginMap;
        out << YAML::Key << "Position" << YAML::Value << component.GetPosition();
        out << YAML::Key << "Rotation" << YAML::Value << component.GetRotation();
        out << YAML::Key << "Scale" << YAML::Value << component.GetScale();
        out << YAML::EndMap;
    }

    void SerilizaCameraComponent(YAML::Emitter &out, CameraComponent &component)
    {
        out << YAML::Key << "CameraComponent";
        out << YAML::BeginMap;
        out << YAML::Key << "Primary" << YAML::Value << component.Primary;
        out << YAML::Key << "FixedAspectRatio" << YAML::Value << component.FixedAspectRatio;
        // Serialize SceneCamera properties if needed
        {
            auto &camera = component.Camera;
            out << YAML::Key << "Camera";
            out << YAML::BeginMap;
            out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera->GetProjectionType();
            out << YAML::Key << "AspectRatio" << YAML::Value << camera->GetAspectRatio();
            // Orthographic properties
            out << YAML::Key << "OrthographicSize" << YAML::Value << camera->GetOrthographicSize();
            out << YAML::Key << "OrthographicNearClip" << YAML::Value << camera->GetOrthographicNearClip();
            out << YAML::Key << "OrthographicFarClip" << YAML::Value << camera->GetOrthographicFarClip();
            // Perspective properties
            out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera->GetPerspectiveFOV();
            out << YAML::Key << "PerspectiveNearClip" << YAML::Value << camera->GetPerspectiveNearClip();
            out << YAML::Key << "PerspectiveFarClip" << YAML::Value << camera->GetPerspectiveFarClip();
            out << YAML::EndMap;
        }
        out << YAML::EndMap;
    }

    void SerilizaSpriteComponent(YAML::Emitter &out, SpriteComponent &component)
    {
        out << YAML::Key << "SpriteComponent";
        out << YAML::BeginMap;
        out << YAML::Key << "Color" << YAML::Value << component.Color;
        if (component.Texture)
        {
            out << YAML::Key << "TexturePath" << YAML::Value << component.Texture->GetPath();
        }
        else
        {
            out << YAML::Key << "TexturePath" << YAML::Value << "None";
        }
        out << YAML::Key << "TilingFactor" << YAML::Value << component.TilingFactor;
        out << YAML::EndMap;
    }

    void SerilizaCircleComponent(YAML::Emitter &out, CircleComponent &component)
    {
        out << YAML::Key << "CircleComponent";
        out << YAML::BeginMap;
        out << YAML::Key << "Color" << YAML::Value << component.Color;
        out << YAML::Key << "Thickness" << YAML::Value << component.Thickness;
        out << YAML::Key << "Fade" << YAML::Value << component.Fade;
        out << YAML::EndMap;
    }

    void SerilizaRigidbody2DComponent(YAML::Emitter &out, Rigidbody2DComponent &component)
    {
        out << YAML::Key << "Rigidbody2DComponent";
        out << YAML::BeginMap;
        out << YAML::Key << "Type" << YAML::Value << (int)component.Type;
        out << YAML::Key << "Velocity" << YAML::Value << component.Velocity;
        out << YAML::Key << "AngularVelocity" << YAML::Value << component.AngularVelocity;
        out << YAML::Key << "FixedRotation" << YAML::Value << component.FixedRotation;
        out << YAML::EndMap;
    }

    void SerilizaBoxCollider2DComponent(YAML::Emitter &out, BoxCollider2DComponent &component)
    {
        out << YAML::Key << "BoxCollider2DComponent";
        out << YAML::BeginMap;
        out << YAML::Key << "Offset" << YAML::Value << component.Offset;
        out << YAML::Key << "Size" << YAML::Value << component.Size;
        out << YAML::Key << "Density" << YAML::Value << component.Density;
        out << YAML::Key << "Friction" << YAML::Value << component.Friction;
        out << YAML::Key << "Restitution" << YAML::Value << component.Restitution;
        out << YAML::Key << "RestitutionThreshold" << YAML::Value << component.RestitutionThreshold;
        out << YAML::EndMap;
    }

    void SerilizaCircleCollider2DComponent(YAML::Emitter &out, CircleCollider2DComponent &component)
    {
        out << YAML::Key << "CircleCollider2DComponent";
        out << YAML::BeginMap;
        out << YAML::Key << "Radius" << YAML::Value << component.Radius;
        out << YAML::Key << "Offset" << YAML::Value << component.Offset;
        out << YAML::Key << "Density" << YAML::Value << component.Density;
        out << YAML::Key << "Friction" << YAML::Value << component.Friction;
        out << YAML::Key << "Restitution" << YAML::Value << component.Restitution;
        out << YAML::Key << "RestitutionThreshold" << YAML::Value << component.RestitutionThreshold;
        out << YAML::EndMap;
    }

    bool DeserializeTagComponent(const YAML::Node &componentNode, TagComponent &component)
    {
        if (componentNode["Tag"])
        {
            component.Tag = componentNode["Tag"].as<std::string>();
            return true;
        }
        else
        {
            LOG_WARN("TagComponent missing Tag during deserialization. Setting to default empty string.");
            component.Tag = "UnNamed Entity";
            return true;
        }
    }

    bool DeserializeTransformComponent(const YAML::Node &componentNode, TransformComponent &component)
    {
        if (componentNode["Position"])
        {
            component.SetPosition(componentNode["Position"].as<glm::vec3>());
        }
        else
        {
            LOG_WARN("TransformComponent missing Position during deserialization. Setting to default (0,0,0).");
            component.SetPosition(glm::vec3(0.0f));
        }
        if (componentNode["Rotation"])
        {
            component.SetRotation(componentNode["Rotation"].as<glm::vec3>());
        }
        else
        {
            LOG_WARN("TransformComponent missing Rotation during deserialization. Setting to default (0,0,0).");
            component.SetRotation(glm::vec3(0.0f));
        }
        if (componentNode["Scale"])
        {
            component.SetScale(componentNode["Scale"].as<glm::vec3>());
        }
        else
        {
            LOG_WARN("TransformComponent missing Scale during deserialization. Setting to default (1,1,1).");
            component.SetScale(glm::vec3(1.0f));
        }
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
        if (componentNode["Camera"])
        {
            auto cameraNode = componentNode["Camera"];
            auto &camera = component.Camera;

            if (cameraNode["ProjectionType"])
            {
                camera->SetProjectionType((SceneCamera::ProjectionType)cameraNode["ProjectionType"].as<int>());
            }
            if (cameraNode["AspectRatio"])
            {
                camera->SetAspectRatio(cameraNode["AspectRatio"].as<float>());
            }
            if (cameraNode["OrthographicSize"])
            {
                camera->SetOrthographicSize(cameraNode["OrthographicSize"].as<float>());
            }
            if (cameraNode["OrthographicNearClip"])
            {
                camera->SetOrthographicNearClip(cameraNode["OrthographicNearClip"].as<float>());
            }
            if (cameraNode["OrthographicFarClip"])
            {
                camera->SetOrthographicFarClip(cameraNode["OrthographicFarClip"].as<float>());
            }
            if (cameraNode["PerspectiveFOV"])
            {
                camera->SetPerspectiveFOV(cameraNode["PerspectiveFOV"].as<float>());
            }
            if (cameraNode["PerspectiveNearClip"])
            {
                camera->SetPerspectiveNearClip(cameraNode["PerspectiveNearClip"].as<float>());
            }
            if (cameraNode["PerspectiveFarClip"])
            {
                camera->SetPerspectiveFarClip(cameraNode["PerspectiveFarClip"].as<float>());
            }
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
                component.Texture = Engine::Renderer::Texture2D::Create(texturePath);
                ENGINE_INFO("Loaded texture from path: {}", texturePath);
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
        if (componentNode["Color"])
        {
            component.Color = componentNode["Color"].as<glm::vec4>();
        }
        if (componentNode["Thickness"])
        {
            component.Thickness = componentNode["Thickness"].as<float>();
        }
        if (componentNode["Fade"])
        {
            component.Fade = componentNode["Fade"].as<float>();
        }
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
        if (componentNode["Offset"])
        {
            component.Offset = componentNode["Offset"].as<glm::vec2>();
        }
        if (componentNode["Size"])
        {
            component.Size = componentNode["Size"].as<glm::vec2>();
        }
        if (componentNode["Density"])
        {
            component.Density = componentNode["Density"].as<float>();
        }
        if (componentNode["Friction"])
        {
            component.Friction = componentNode["Friction"].as<float>();
        }
        if (componentNode["Restitution"])
        {
            component.Restitution = componentNode["Restitution"].as<float>();
        }
        if (componentNode["RestitutionThreshold"])
        {
            component.RestitutionThreshold = componentNode["RestitutionThreshold"].as<float>();
        }
        return true;
    }

    bool DeserializeCircleCollider2DComponent(const YAML::Node &componentNode, CircleCollider2DComponent &component)
    {
        if (componentNode["Radius"])
        {
            component.Radius = componentNode["Radius"].as<float>();
        }
        if (componentNode["Offset"])
        {
            component.Offset = componentNode["Offset"].as<glm::vec2>();
        }
        if (componentNode["Density"])
        {
            component.Density = componentNode["Density"].as<float>();
        }
        if (componentNode["Friction"])
        {
            component.Friction = componentNode["Friction"].as<float>();
        }
        if (componentNode["Restitution"])
        {
            component.Restitution = componentNode["Restitution"].as<float>();
        }
        if (componentNode["RestitutionThreshold"])
        {
            component.RestitutionThreshold = componentNode["RestitutionThreshold"].as<float>();
        }
        return true;
    }

} // namespace Engine::Serialization
