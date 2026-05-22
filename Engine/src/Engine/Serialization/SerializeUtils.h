#pragma once
#include "yaml-cpp/yaml.h"
#include <glm/glm.hpp>
#include <filesystem>
#include "Engine/Core/UUID.h"
#include "Engine/Project/Project.h"
#include "Engine/Resource/ResourceRef.h"
#include "Engine/Scene/EntityRef.h"
namespace YAML
{
    // Serialize to YAML
    Emitter &operator<<(Emitter &out, const glm::vec2 &vec);
    Emitter &operator<<(Emitter &out, const glm::vec3 &vec);
    Emitter &operator<<(Emitter &out, const glm::vec4 &vec);
    Emitter &operator<<(Emitter &out, const Engine::EntityRef &entityRef);

    // Deserialize from YAML
    template <>
    struct convert<glm::vec2>
    {
        static Node encode(const glm::vec2 &rhs);
        static bool decode(const Node &node, glm::vec2 &rhs);
    };

    template <>
    struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3 &rhs);
        static bool decode(const Node &node, glm::vec3 &rhs);
    };

    template <>
    struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4 &rhs);
        static bool decode(const Node &node, glm::vec4 &rhs);
    };

    template <>
    struct convert<Engine::UUID>
    {
        static Node encode(const Engine::UUID &uuid);

        static bool decode(const Node &node, Engine::UUID &uuid);
    };

    template <typename T>
    struct convert<Engine::ResourceRef<T>>
    {
        static Node encode(const Engine::ResourceRef<T> &ref)
        {
            Node node;
            std::string serializedPath = ref.path;
            if (!serializedPath.empty() && Engine::Project::GetActiveProject())
            {
                std::filesystem::path relativePath = Engine::Project::GetPathRelativeToAssets(serializedPath);
                serializedPath = relativePath.generic_string();
            }
            node["path"] = serializedPath;
            return node;
        }

        static bool decode(const Node &node, Engine::ResourceRef<T> &ref)
        {
            if (!node.IsMap() || !node["path"])
                return false;

            ref.path = node["path"].as<std::string>();
            ref.instance = nullptr; // 反序列化时不加载资源，只设置路径，实际加载由资源管理器负责
            return true;
        }
    };

    template <>
    struct convert<Engine::EntityRef>
    {
        static Node encode(const Engine::EntityRef &entityRef);

        static bool decode(const Node &node, Engine::EntityRef &entityRef);
    };
} // namespace YAML
