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
            return true;
        }
        else
        {
            LOG_WARN("SpriteComponent missing Color during deserialization. Setting to default white color.");
            component.Color = glm::vec4(1.0f);
            return true;
        }
    }

} // namespace Engine::Serialization
