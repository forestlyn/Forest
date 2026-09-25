#pragma once
#include <filesystem>
#include <string>

namespace Engine
{
    // Shared by the editor and the development runner; all paths come from the active project.
    class ProjectScripts
    {
    public:
        static std::filesystem::path AssemblyPath();
        static std::filesystem::path SourceDirectory();
        static bool Build(const std::filesystem::path &coreAssembly, std::string &error);
        static bool CanLoad(const std::filesystem::path &assembly, std::string &error);
    };
}
