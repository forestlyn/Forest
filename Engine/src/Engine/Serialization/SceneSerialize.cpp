#include "SceneSerialize.h"
#include "EntitySerialize.h"
#include <yaml-cpp/yaml.h>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <vector>
namespace fs = std::filesystem;
namespace Engine::Serialization
{
    SceneSerialize::SceneSerialize(const Ref<Scene> &scene)
        : m_Scene(scene)
    {
    }

    SceneSerialize::~SceneSerialize()
    {
    }

    bool createDirectoriesIfNotExists(const std::string &path)
    {
        try
        {
            if (!fs::exists(path))
            {
                return fs::create_directories(path);
            }
            return true; // 已存在
        }
        catch (const fs::filesystem_error &e)
        {
            LOG_ERROR("Filesystem error: {}", e.what());
            return false;
        }
    }

    void SceneSerialize::Serialize(const std::string &filePath)
    {
        if (filePath.empty() || filePath.size() == 0)
        {
            LOG_ERROR("File path is empty for scene serialization.");
            return;
        }

        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene";
        out << YAML::Value << "Untitled";
        out << YAML::Key << "Entities";
        out << YAML::BeginSeq;
        std::vector<Entity> entities;
        auto view = m_Scene->GetRegistry().view<TagComponent>();
        for (auto entity : view)
        {
            entities.emplace_back(entity, m_Scene.get());
        }

        std::sort(entities.begin(), entities.end(), [](Entity lhs, Entity rhs)
                  { return (uint64_t)lhs.GetUUID() < (uint64_t)rhs.GetUUID(); });

        for (auto entity : entities)
        {
            SerializeEntity(out, entity);
        }
        out << YAML::EndSeq;
        out << YAML::EndMap;

        if (createDirectoriesIfNotExists(fs::path(filePath).parent_path().string()) == false)
        {
            LOG_ERROR("Failed to create directories for scene serialization: {}", filePath);
            return;
        }
        std::ofstream fout(filePath);
        if (!fout.is_open())
        {
            LOG_ERROR("Failed to open file for scene serialization: {}", filePath);
            return;
        }
        fout << out.c_str();
    }

    bool SceneSerialize::Deserialize(const std::string &filePath)
    {
        if (filePath.empty() || filePath.size() == 0)
        {
            LOG_ERROR("File path is empty for scene deserialization.");
            return false;
        }

        std::ifstream stream(filePath);
        if (!stream.is_open())
        {
            LOG_ERROR("Failed to open file for scene deserialization: {}", filePath);
            return false;
        }

        YAML::Node data;

        try
        {
            data = YAML::Load(stream);
        }
        catch (const YAML::ParserException &e)
        {
            LOG_ERROR("YAML Parser Exception: {} at line {}, column {}", e.what(), e.mark.line + 1, e.mark.column + 1);
            return false;
        }
        if (!data["Scene"])
        {
            LOG_ERROR("Scene node not found in YAML file: {}", filePath);
            return false;
        }
        auto entities = data["Entities"];
        if (entities)
        {
            for (auto entityNode : entities)
            {
                uint64_t uuid = entityNode["EntityID"].as<uint64_t>();
                Entity entity = m_Scene->CreateEntityWithID(UUID(uuid));
                if (!DeserializeEntity(entityNode, entity))
                {
                    LOG_ERROR("Failed to deserialize entity with ID: {}", uuid);
                }
            }
        }
        return true;
    }
} // namespace Engine::Serialization
