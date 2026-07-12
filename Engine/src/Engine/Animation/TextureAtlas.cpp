#include "TextureAtlas.h"
#include "Engine/Renderer/Shader/Texture.h"
#include "Engine/Resource/ResourceManager.h"
#include "Engine/Core/Core.h"
#include <fstream>
#include "yaml-cpp/yaml.h"

namespace Engine
{
    namespace
    {
        template <typename T>
        T ReadYamlValue(const YAML::Node &node, const char *key, T defaultValue)
        {
            if (!node)
            {
                return defaultValue;
            }

            const YAML::Node value = node[key];
            if (!value)
            {
                return defaultValue;
            }

            try
            {
                return value.as<T>();
            }
            catch (const YAML::Exception &exception)
            {
                ENGINE_WARN("Failed to read yaml key '{}': {}", key, exception.what());
                return defaultValue;
            }
        }

        glm::vec2 ReadYamlVec2(const YAML::Node &node, const char *xKey, const char *yKey, glm::vec2 defaultValue)
        {
            if (!node)
            {
                return defaultValue;
            }

            return {
                ReadYamlValue<float>(node, xKey, defaultValue.x),
                ReadYamlValue<float>(node, yKey, defaultValue.y)};
        }
    }

    Ref<TextureAtlas> TextureAtlas::Create(const std::string &configPath)
    {
        if (configPath.empty())
        {
            ENGINE_ERROR("Texture atlas config path cannot be empty.");
            return nullptr;
        }

        auto atlas = CreateRef<TextureAtlas>();
        std::ifstream file(configPath);
        if (!file.is_open())
        {
            ENGINE_ERROR("Failed to open texture atlas config file: {}", configPath);
            return nullptr;
        }

        YAML::Node yamlData;
        try
        {
            yamlData = YAML::Load(file);
        }
        catch (const YAML::Exception &exception)
        {
            ENGINE_ERROR("Failed to parse texture atlas yaml '{}': {}", configPath, exception.what());
            return nullptr;
        }

        // Load texure
        std::string imageFileName = yamlData["meta"]["image"].as<std::string>();
        if (imageFileName.empty())
        {
            ENGINE_ERROR("Texture atlas yaml '{}' must specify an image file in 'meta.image'.", configPath);
            return nullptr;
        }
        auto texture = ResourceManager::Get()->GetOrLoad<Renderer::Texture2D>(imageFileName);
        if (!texture)
        {
            ENGINE_ERROR("Failed to load texture '{}' for texture atlas '{}'.", imageFileName, configPath);
            return nullptr;
        }
        atlas->m_Texture = texture;

        const YAML::Node framesNode = yamlData["frames"];
        if (!framesNode || !framesNode.IsMap())
        {
            ENGINE_ERROR("Texture atlas yaml '{}' must contain a 'frames' map.", configPath);
            return nullptr;
        }

        const float textureWidth = static_cast<float>(texture->GetWidth());
        const float textureHeight = static_cast<float>(texture->GetHeight());
        if (textureWidth <= 0.0f || textureHeight <= 0.0f)
        {
            ENGINE_ERROR("Texture atlas '{}' has invalid texture size: {}x{}", configPath, texture->GetWidth(), texture->GetHeight());
            return nullptr;
        }

        const std::string texturePath = texture->GetPath();

        for (const auto &frameEntry : framesNode)
        {
            std::string frameName;
            try
            {
                frameName = frameEntry.first.as<std::string>();
            }
            catch (const YAML::Exception &exception)
            {
                ENGINE_WARN("Skipping texture atlas frame with invalid name in '{}': {}", configPath, exception.what());
                continue;
            }

            const YAML::Node frameData = frameEntry.second;
            if (!frameData || !frameData.IsMap())
            {
                ENGINE_WARN("Skipping malformed texture atlas frame '{}' in '{}'.", frameName, configPath);
                continue;
            }

            const YAML::Node rectNode = frameData["frame"];
            if (!rectNode || !rectNode.IsMap())
            {
                ENGINE_WARN("Skipping texture atlas frame '{}' because it has no frame rect.", frameName);
                continue;
            }

            const float x = ReadYamlValue<float>(rectNode, "x", 0.0f);
            const float y = ReadYamlValue<float>(rectNode, "y", 0.0f);
            const float width = ReadYamlValue<float>(rectNode, "w", 0.0f);
            const float height = ReadYamlValue<float>(rectNode, "h", 0.0f);
            if (width <= 0.0f || height <= 0.0f)
            {
                ENGINE_WARN("Skipping texture atlas frame '{}' because its size is invalid: {}x{}.", frameName, width, height);
                continue;
            }

            SpriteFrame frame;
            frame.Name = frameName;
            frame.TextureRef.path = texturePath;
            frame.TextureRef.instance = texture;
            frame.IsRotated = ReadYamlValue<bool>(frameData, "rotated", false);
            if (frame.IsRotated)
            {
                ENGINE_WARN("Texture atlas frame '{}' is marked rotated. Rotated frames are not fully supported yet.", frameName);
            }

            const YAML::Node spriteSourceSizeNode = frameData["spriteSourceSize"];
            frame.Offset = ReadYamlVec2(spriteSourceSizeNode, "x", "y", {0.0f, 0.0f});
            frame.Size = {
                ReadYamlValue<float>(spriteSourceSizeNode, "w", width),
                ReadYamlValue<float>(spriteSourceSizeNode, "h", height)};

            const YAML::Node sourceSizeNode = frameData["sourceSize"];
            frame.OriginalSize = {
                ReadYamlValue<float>(sourceSizeNode, "w", width),
                ReadYamlValue<float>(sourceSizeNode, "h", height)};

            frame.Pivot = ReadYamlVec2(frameData["pivot"], "x", "y", {0.5f, 0.5f});

            const float u0 = x / textureWidth;
            const float u1 = (x + width) / textureWidth;
            const float v0 = 1.0f - ((y + height) / textureHeight);
            const float v1 = 1.0f - (y / textureHeight);
            frame.UVMin = {u0, v0};
            frame.UVMax = {u1, v1};

            atlas->m_Frames[frame.Name] = frame;
            atlas->m_FrameNames.push_back(frame.Name);
        }

        if (atlas->m_FrameNames.empty())
        {
            ENGINE_ERROR("Texture atlas yaml '{}' did not contain any valid frames.", configPath);
            return nullptr;
        }

        return atlas;
    }
}
