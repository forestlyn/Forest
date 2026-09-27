#include "RuntimePaths.h"
#include "Engine/Project/Project.h"
#include <Windows.h>
#include <stdexcept>
#include <vector>

namespace fs = std::filesystem;
namespace Engine::Core
{
    namespace
    {
        fs::path resourcesRoot, dataRoot;

        fs::path ExecutablePath()
        {
            std::vector<wchar_t> buffer(512);
            for (;;)
            {
                DWORD count = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
                if (!count) throw std::runtime_error("Cannot locate executable directory");
                if (count < buffer.size()) return fs::path(std::wstring(buffer.data(), count));
                buffer.resize(buffer.size() * 2);
            }
        }
    }

    fs::path RuntimePaths::ExecutableDirectory()
    {
        return ExecutablePath().parent_path();
    }

    void RuntimePaths::Configure(const fs::path &resources, const fs::path &userData)
    {
        resourcesRoot = fs::absolute(resources.empty() ? ExecutableDirectory() / "resources" : resources).lexically_normal();
        dataRoot = userData.empty() ? fs::path{} : fs::absolute(userData).lexically_normal();
    }

    fs::path RuntimePaths::Resources()
    {
        return resourcesRoot.empty() ? ExecutableDirectory() / "resources" : resourcesRoot;
    }

    fs::path RuntimePaths::UserData()
    {
        if (dataRoot.empty())
        {
            DWORD length = GetEnvironmentVariableW(L"LOCALAPPDATA", nullptr, 0);
            if (!length) throw std::runtime_error("LOCALAPPDATA is unavailable; specify a user data directory");
            std::vector<wchar_t> buffer(length);
            if (!GetEnvironmentVariableW(L"LOCALAPPDATA", buffer.data(), length))
                throw std::runtime_error("Cannot locate user data directory");
            dataRoot = fs::path(buffer.data()) / "Forest" / ExecutablePath().stem();
        }
        fs::create_directories(dataRoot);
        return dataRoot;
    }

    fs::path RuntimePaths::Cache()
    {
        auto path = UserData() / "Cache";
        fs::create_directories(path);
        return path;
    }

    fs::path RuntimePaths::Logs()
    {
        auto path = UserData() / "Logs";
        fs::create_directories(path);
        return path;
    }

    fs::path RuntimePaths::EngineResource(const fs::path &relativePath)
    {
        return (Resources() / relativePath).lexically_normal();
    }

    fs::path RuntimePaths::ResolveAsset(const fs::path &path)
    {
        if (path.empty()) return {};
        if (path.is_absolute()) return fs::weakly_canonical(path);
        const auto normalized = path.lexically_normal().generic_string();
        if (normalized.rfind("resources/", 0) == 0)
            return fs::weakly_canonical(EngineResource(normalized.substr(10)));
        if (Project::GetActiveProject())
            return fs::weakly_canonical(Project::GetActiveProjectAssetPath(path));
        return fs::weakly_canonical(ExecutableDirectory() / path);
    }
}
