#include "Utils.h"
namespace EngineEditor
{
    std::string Utils::ExtraNameFromPath(std::filesystem::path &path)
    {
        if (std::filesystem::is_directory(path))
        {
            // 如果是目录，返回目录名
            return path.filename().string();
        }
        else
        {
            // 如果是文件，返回不含扩展名的文件名
            return path.stem().string();
        }
    }
}