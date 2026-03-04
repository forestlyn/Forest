#include "Project.h"
#include <fstream>
#include "Engine/Serialization/ProjectSerialize.h"
namespace Engine
{
    Ref<Project> Project::s_ActiveProject = nullptr;
    Ref<Project> Project::Create()
    {
        Ref<Project> project = CreateRef<Project>();
        s_ActiveProject = project;
        return project;
    }

    Ref<Project> Project::Load(const std::filesystem::path &filepath)
    {
        Ref<Project> project = CreateRef<Project>();
        Serialization::ProjectSerialize serializer(project);
        if (!serializer.Deserialize(filepath))
        {
            LOG_ERROR("Failed to load project file: {0}", filepath.string());
            return nullptr;
        }
        s_ActiveProject = project;
        s_ActiveProject->m_ProjectDirectory = filepath.parent_path();
        ENGINE_INFO("Loaded project: {0} {1}", project->GetActiveProjectName(), filepath.string());
        return project;
    }
    bool Project::SaveActiveProject(const std::filesystem::path &filepath)
    {

        Serialization::ProjectSerialize serializer(s_ActiveProject);
        if (!serializer.Serialize(filepath))
        {
            LOG_ERROR("Failed to save project file: {0}", filepath.string());
            return false;
        }
        return true;
    }
}