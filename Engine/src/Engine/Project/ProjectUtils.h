#pragma once
#include "Engine/pcheader.h"
#include <filesystem>
namespace Engine
{
    class ProjectUtils
    {
    public:
        static bool CreateProject(std::string projectName, std::filesystem::path path);
    };
}