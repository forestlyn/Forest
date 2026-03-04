#include "ProjectSerialize.h"
#include "yaml-cpp/yaml.h"
#include <fstream>
namespace Engine::Serialization
{
    ProjectSerialize::ProjectSerialize(Ref<Project> project)
    {
        m_Project = project;
    }
    bool ProjectSerialize::Serialize(const std::filesystem::path &filepath)
    {
        YAML::Emitter out;
        const ProjectSettings &settings = m_Project->GetProjectSettings();
        out << YAML::BeginMap;
        out << YAML::Key << "Name" << YAML::Value << settings.Name;
        out << YAML::Key << "AssetDirectory" << YAML::Value << settings.AssetDirectory;
        out << YAML::Key << "StartScene" << YAML::Value << settings.StartScene;
        out << YAML::Key << "ScriptDLLDirectory" << YAML::Value << settings.ScriptDLLDirectory;
        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();
        return true;
    }

    bool ProjectSerialize::Deserialize(const std::filesystem::path &filepath)
    {
        std::ifstream fin(filepath);
        if (!fin.is_open())
        {
            LOG_ERROR("Failed to open project file: {0}", filepath.string());
            return false;
        }
        YAML::Node node = YAML::Load(fin);
        ProjectSettings &settings = m_Project->GetProjectSettings();
        if (node["Name"])
            settings.Name = node["Name"].as<std::string>();
        if (node["AssetDirectory"])
            settings.AssetDirectory = node["AssetDirectory"].as<std::string>();
        if (node["StartScene"])
            settings.StartScene = node["StartScene"].as<std::string>();
        if (node["ScriptDLLDirectory"])
            settings.ScriptDLLDirectory = node["ScriptDLLDirectory"].as<std::string>();
        return true;
    }
}