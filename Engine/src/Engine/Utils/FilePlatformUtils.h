#pragma once
#include <string>
namespace Engine
{
    class FileDialog
    {
    public:
        static std::string OpenFileDialog(const char *filter = "All Files\0*.*\0", const std::string &initialPath = "");
        static std::string SaveFileDialog(const char *filter = "All Files\0*.*\0", const std::string &initialPath = "");
    };
} // namespace Engine