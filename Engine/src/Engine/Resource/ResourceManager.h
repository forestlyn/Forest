#pragma once
#include "Engine/Renderer/Shader/Texture.h"
#include "Engine/Project/Project.h"
#include "ResourceRef.h"
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <algorithm>
#include <filesystem>
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
            if (normalizedPath.empty())
                return normalizedPath;

            std::filesystem::path path(normalizedPath);
            if (path.is_absolute() || IsEngineResourcePath(normalizedPath))
            {
                return normalizedPath;
            }

            if (Project::GetActiveProject())
            {
                auto absolutePath = Project::GetActiveProjectAssetPath(normalizedPath);
                return NormalizePath(absolutePath.string());
            }

            return normalizedPath;
        }

        // 核心接口
        template <typename T>
        Ref<T> GetOrLoad(const std::string &rawPath)
        {
            if (rawPath.empty())
                return nullptr;

            std::string cacheKey = NormalizePath(rawPath);
            std::string loadPath = ResolvePathForLoad(cacheKey);

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
                cachedResource.path = cacheKey;
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
}
