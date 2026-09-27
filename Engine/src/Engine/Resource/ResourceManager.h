#pragma once
#include "Engine/Core/FileSystem.h"
#include "Engine/Renderer/Shader/Texture.h"
#include "Engine/Animation/TextureAtlas.h"
#include "Engine/Animation/AnimationClip2D.h"
#include "Engine/Project/Project.h"
#include "ResourceRef.h"
#include "Engine/Core/RuntimePaths.h"
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <algorithm>
namespace Engine
{
    class ResourceManager
    {
    private:
        static ResourceManager *s_Instance;

        std::unordered_map<std::string, ResourceRef<void>> m_Cache;

        std::unordered_set<std::string> m_LostResources; // 记录丢失/损坏的资源路径

    public:
        static ResourceManager *Get() { return s_Instance; }

        static std::string NormalizePath(std::string path)
        {
            std::replace(path.begin(), path.end(), '\\', '/');
            return path;
        }

        static bool IsEngineResourcePath(const std::string &path)
        {
            if (path.empty())
                return false;

            std::string normalized = NormalizePath(path);
            return normalized.rfind("resources/", 0) == 0;
        }

        static std::string ResolvePathForLoad(const std::string &normalizedPath)
        {
            return Core::RuntimePaths::ResolveAsset(normalizedPath).generic_string();
        }

        // 核心接口
        template <typename T>
        Ref<T> GetOrLoad(const std::string &rawPath)
        {
            if (rawPath.empty())
                return nullptr;

            std::string loadPath = ResolvePathForLoad(NormalizePath(rawPath));
            // Both successful and failed loads are scoped to the resolved file and resource type.
            std::string cacheKey = std::string(typeid(T).name()) + ":" + loadPath;

            auto it = m_Cache.find(cacheKey);
            if (it != m_Cache.end())
            {
                return std::static_pointer_cast<T>(it->second.instance);
            }

            auto lostIt = m_LostResources.find(cacheKey);
            if (lostIt != m_LostResources.end())
            {
                // ENGINE_WARN("Resource previously failed to load: {0}", path);
                return nullptr;
            }

            Ref<T> newResource = LoadAssetFromFile<T>(loadPath);

            if (newResource)
            {
                ResourceRef<void> cachedResource;
                cachedResource.path = loadPath;
                cachedResource.instance = newResource;
                m_Cache[cacheKey] = cachedResource; // 存入缓存
                return newResource;
            }
            else
            {
                ENGINE_ERROR("Failed to load resource: {0}", cacheKey);
                m_LostResources.insert(cacheKey);
            }

            return nullptr;
        }

    private:
        template <typename T>
        Ref<T> LoadAssetFromFile(const std::string &path);
    };

    template <>
    inline Ref<Renderer::Texture2D> ResourceManager::LoadAssetFromFile<Renderer::Texture2D>(const std::string &path)
    {
        auto texture = Renderer::Texture2D::Create(path);
        return texture;
    }

    template <>
    inline Ref<TextureAtlas> ResourceManager::LoadAssetFromFile<TextureAtlas>(const std::string &path)
    {
        auto textureAtlas = TextureAtlas::Create(path);
        return textureAtlas;
    }

    template <>
    inline Ref<SpriteFrame> ResourceManager::LoadAssetFromFile<SpriteFrame>(const std::string &path)
    {
        ENGINE_ERROR("Unsupported resource type for path: {0}", path);
        return nullptr;
    }

    template <>
    inline Ref<AnimationClip2D> ResourceManager::LoadAssetFromFile<AnimationClip2D>(const std::string &path)
    {
        if (path.empty())
        {
            ENGINE_ERROR("AnimationClip2D path is empty.");
            return nullptr;
        }
        Ref<AnimationClip2D> clip = CreateRef<AnimationClip2D>();

        YAML::Node yamlData = Core::FileSystem::ReadYamlFile(path);
        if (!yamlData || !yamlData.IsMap())
        {
            ENGINE_ERROR("Failed to load AnimationClip2D from '{}': Invalid YAML format.", path);
            return nullptr;
        }

        clip->name = yamlData["name"].as<std::string>("");
        clip->loop = yamlData["loop"].as<bool>(true);

        std::string atlasRelativePath = yamlData["atlas"].as<std::string>();
        auto atlas = ResourceManager::Get()->GetOrLoad<TextureAtlas>(atlasRelativePath);
        if (!atlas)
        {
            ENGINE_ERROR("Failed to load atlas {0} for animation {1}", atlasRelativePath, path);
            return nullptr;
        }

        const YAML::Node framesNode = yamlData["frames"];
        if (!framesNode || !framesNode.IsSequence())
        {
            ENGINE_ERROR("AnimationClip2D '{}' has no valid 'frames' sequence.", path);
            return nullptr;
        }
        for (const auto &frameNode : framesNode)
        {
            if (!frameNode || !frameNode.IsMap())
            {
                ENGINE_WARN("Skipping invalid frame in AnimationClip2D '{}'.", path);
                continue;
            }

            AnimFrame animFrame;
            std::string frameName = frameNode["frame"].as<std::string>("");
            animFrame.Duration = frameNode["duration"].as<float>(0.1f); // 默认持续时间为 0.1 秒

            if (frameName.empty())
            {
                ENGINE_WARN("Skipping frame with empty name in AnimationClip2D '{}'.", path);
                continue;
            }

            // Load the SpriteFrame using ResourceManager
            animFrame.Frame = atlas->GetFrame(frameName) ? *atlas->GetFrame(frameName) : SpriteFrame();
            clip->frames.push_back(animFrame);
        }
        return clip;
    }
}