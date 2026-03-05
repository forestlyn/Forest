#pragma once
#include "Engine/Core/Core.h"
#include <filesystem>
namespace Engine
{
    struct ProjectSettings
    {
        std::string Name = "New Project";
        std::string AssetDirectory = "Assets";
        std::string StartScene = "Scenes/Sample.scene";
        std::string ScriptDLLDirectory;
    };
    class Project
    {
    public:
        static Ref<Project> Create(std::string projectname, const std::filesystem::path &filepath);
        static Ref<Project> Load(const std::filesystem::path &filepath);
        static bool SaveActiveProject(const std::filesystem::path &filepath);

        static Ref<Project> GetActiveProject()
        {
            return s_ActiveProject;
        }

        static std::filesystem::path GetActiveProjectDirectory()
        {
            return s_ActiveProject->m_ProjectDirectory;
        }
        static std::string GetActiveProjectName()
        {
            return s_ActiveProject->m_Settings.Name;
        }
        static std::filesystem::path GetActiveProjectAssetDirectory()
        {
            return GetActiveProjectDirectory() / s_ActiveProject->m_Settings.AssetDirectory;
        }
        static std::filesystem::path GetActiveProjectStartScene()
        {
            return GetActiveProjectAssetDirectory() / s_ActiveProject->m_Settings.StartScene;
        }
        static std::filesystem::path GetActiveProjectScriptDLLDirectory()
        {
            return GetActiveProjectAssetDirectory() / s_ActiveProject->m_Settings.ScriptDLLDirectory;
        }

        static std::filesystem::path GetActiveProjectAssetPath(std::filesystem::path relativePath)
        {
            return GetActiveProjectAssetDirectory() / relativePath;
        }

        ProjectSettings &GetProjectSettings()
        {
            return m_Settings;
        }

    private:
        ProjectSettings m_Settings;
        std::filesystem::path m_ProjectDirectory;
        static Ref<Project> s_ActiveProject;
    };
}