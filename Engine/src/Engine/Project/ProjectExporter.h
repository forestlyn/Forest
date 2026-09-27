#pragma once
#include <filesystem>
#include <string>

namespace Engine
{
    class ProjectExporter
    {
    public:
        static std::filesystem::path DefaultRuntimeDirectory();
        // Exports the active project to a NEW directory, without changing its settings or script outputs.
        static bool Export(const std::filesystem::path &outputDirectory,
                           const std::filesystem::path &runtimeDirectory, std::string &error);
    };
}
