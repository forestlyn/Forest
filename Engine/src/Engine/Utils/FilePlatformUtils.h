#pragma once
#include <string>
namespace Engine
{
    class FileDialog
    {
    public:
        static std::string OpenFileDialog(const char *filter = "All Files\0*.*\0", const std::string &initialPath = "");
        static std::string SaveFileDialog(const char *filter = "All Files\0*.*\0", const std::string &initialPath = "");
        static std::string OpenFolderDialog(const std::string &initialPath = "", const wchar_t *title = L"选择新建项目的保存目录");
    };
} // namespace Engine
