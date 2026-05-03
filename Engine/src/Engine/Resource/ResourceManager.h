#pragma once
#include "Engine/Renderer/Shader/Texture.h"
#include "ResourceRef.h"
#include <unordered_map>
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

    public:
        static ResourceManager *Get() { return s_Instance; }

        static std::string NormalizePath(std::string path)
        {
            std::replace(path.begin(), path.end(), '\\', '/');
            return path;
        }

        // 核心接口
        template <typename T>
        Ref<T> GetOrLoad(const std::string &rawPath)
        {
            if (rawPath.empty())
                return nullptr;

            std::string path = NormalizePath(rawPath);

            auto it = m_Cache.find(path);
            if (it != m_Cache.end())
            {
                return std::static_pointer_cast<T>(it->second.instance);
            }

            Ref<T> newResource = LoadAssetFromFile<T>(path);

            if (newResource)
            {
                ResourceRef<void> cachedResource;
                cachedResource.path = path;
                cachedResource.instance = newResource;
                m_Cache[path] = cachedResource; // 存入缓存
                return newResource;
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
