#include "ProjectUtils.h"
#include "Engine/Core/RuntimePaths.h"
#include <fstream>
namespace Engine
{


    bool ProjectUtils::CreateProject(std::string projectName, std::filesystem::path path)
    {
        if (path.empty())
        {
            ENGINE_ERROR("project path is empty!");
            return false;
        }
        if (!std::filesystem::is_directory(path))
        {
            ENGINE_ERROR("project path isn't directory!");
            return false;
        }
        std::filesystem::path projectDir = path;
        ENGINE_INFO("projectDir:{}", projectDir.string());
        if (std::filesystem::exists(projectDir) || std::filesystem::create_directory(projectDir))
        {
            std::filesystem::copy_options copyOptions = std::filesystem::copy_options::recursive | std::filesystem::copy_options::skip_existing;
            std::filesystem::copy(Core::RuntimePaths::EngineResource("template/project"), projectDir, copyOptions);
            return true;
        }
        else
        {
            ENGINE_ERROR("CreateProject Failed!");
        }
        return false;
    }
}