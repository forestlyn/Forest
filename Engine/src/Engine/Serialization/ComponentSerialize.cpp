#include "ComponentSerialize.h"
#include "Engine/Core/Core.h"
#include "Engine/Scripts/ScriptEngine.h"
#include "Engine/Project/Project.h"
#include "Engine/Reflection/Reflect.h"

namespace Engine::Serialization
{

    void SerializeComponentImpl(YAML::Emitter &out, const SpriteComponent &component)
    {
        ENGINE_INFO("Serializing SpriteComponent");
        out << YAML::Key << "SpriteComponent";
        out << YAML::BeginMap;
        out << YAML::Key << "Color" << YAML::Value << component.Color;
        out << YAML::Key << "TilingFactor" << YAML::Value << component.TilingFactor;
        out << YAML::Key << "TexturePath" << YAML::Value << (component.Texture ? component.Texture->GetPath() : "None");
        out << YAML::EndMap;
    }

    bool DeserializeComponentImpl(const YAML::Node &componentNode, SpriteComponent &component)
    {
        ENGINE_INFO("Deserializing SpriteComponent");
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
} // namespace Engine::Serialization
