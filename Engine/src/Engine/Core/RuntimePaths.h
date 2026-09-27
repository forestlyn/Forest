#pragma once
#include <filesystem>
#include <string>

namespace Engine::Core
{
    class RuntimePaths
    {
    public:
        static std::filesystem::path ExecutableDirectory();
        static void Configure(const std::filesystem::path &resources = {}, const std::filesystem::path &userData = {});
        static std::filesystem::path Resources();
        static std::filesystem::path UserData();
        static std::filesystem::path Cache();
        static std::filesystem::path Logs();
        // resources/... is reserved for engine assets; other relative paths use the active project.
        static std::filesystem::path ResolveAsset(const std::filesystem::path &path);
        static std::filesystem::path EngineResource(const std::filesystem::path &relativePath);
    };
}
