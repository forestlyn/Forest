#include "Engine/Utils/FilePlatformUtils.h"

#include <windows.h>
#include <commdlg.h>
#include <glfw/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>
#include "Engine/Core/Application.h"
namespace Engine
{
    std::string FileDialog::OpenFileDialog(const char *filter, const std::string &initialPath)
    {
        OPENFILENAMEA ofn;      // common dialog box structure
        char szFile[260] = {0}; // buffer for file name

        // Initialize OPENFILENAME
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = glfwGetWin32Window((GLFWwindow *)Core::Application::Get().GetWindow().GetNativeWindow());
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.lpstrInitialDir = initialPath.empty() ? nullptr : initialPath.c_str();
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        // Display the Open dialog box
        if (GetOpenFileNameA(&ofn) == TRUE)
        {
            return std::string(ofn.lpstrFile);
        }
        return std::string();
    }

    std::string FileDialog::SaveFileDialog(const char *filter, const std::string &initialPath)
    {
        OPENFILENAMEA ofn;      // common dialog box structure
        char szFile[260] = {0}; // buffer for file name

        // Initialize OPENFILENAME
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = glfwGetWin32Window((GLFWwindow *)Core::Application::Get().GetWindow().GetNativeWindow());
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.lpstrInitialDir = initialPath.empty() ? nullptr : initialPath.c_str();
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

        // Display the Save dialog box
        if (GetSaveFileNameA(&ofn) == TRUE)
        {
            return std::string(ofn.lpstrFile);
        }
        return std::string();
    }
}