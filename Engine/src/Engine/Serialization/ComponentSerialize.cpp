#include "ComponentSerialize.h"
#include "Engine/Core/Core.h"
#include "Engine/Reflection/Reflect.h"

namespace Engine::Serialization
{
    bool DeserializeComponentImpl(const YAML::Node &componentNode, SpriteComponent &component)
    {
        ENGINE_INFO("Deserializing SpriteComponent");

        DESERILIZE_COMPONENT(componentNode, component, SpriteComponent);

        if (!componentNode["TextureRef"] && componentNode["TexturePath"])
        {
            std::string texturePath = componentNode["TexturePath"].as<std::string>();
            if (texturePath.empty() || texturePath == "None")
            {
                component.TextureRef.Clear();
            }
            else
            {
                component.TextureRef.path = texturePath;
                component.TextureRef.instance = nullptr;
            }
        }

        return true;
    }
} // namespace Engine::Serialization
