#pragma once
#include "Engine/Project/Project.h"
namespace Engine::Serialization
{
    class ProjectSerialize
    {
    public:
        ProjectSerialize(Ref<Project> project);
        bool Serialize(const std::filesystem::path &filepath);
        bool Deserialize(const std::filesystem::path &filepath);

    private:
        Ref<Project> m_Project;
    };
}