#include "Engine/Utils/FilePlatformUtils.h"

#include <windows.h>
#include <commdlg.h>
#include <glfw/glfw3.h>
#include <shlobj.h>
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
        ofn.lpstrDefExt = "yaml";
        ofn.lpstrInitialDir = initialPath.empty() ? nullptr : initialPath.c_str();
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

        // Display the Save dialog box
        if (GetSaveFileNameA(&ofn) == TRUE)
        {
            return std::string(ofn.lpstrFile);
        }
        return std::string();
    }

    std::string FileDialog::OpenFolderDialog(const std::string &initialPath)
    {
        BROWSEINFOW bi = {0};
        wchar_t path[MAX_PATH] = {0};
        wchar_t displayName[MAX_PATH] = {0};

        // 获取窗口句柄
        bi.hwndOwner = glfwGetWin32Window((GLFWwindow *)Core::Application::Get().GetWindow().GetNativeWindow());
        bi.pszDisplayName = displayName;
        bi.lpszTitle = L"选择项目目录";
        bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;

        // 如果有初始路径，设置初始目录
        if (!initialPath.empty())
        {
            // 将 UTF-8 字符串转换为宽字符串
            int len = MultiByteToWideChar(CP_UTF8, 0, initialPath.c_str(), -1, NULL, 0);
            std::wstring wInitialPath(len, 0);
            MultiByteToWideChar(CP_UTF8, 0, initialPath.c_str(), -1, &wInitialPath[0], len);

            // 创建 ITEMIDLIST 从路径
            LPSHELLFOLDER pDesktop = NULL;
            SHGetDesktopFolder(&pDesktop);

            if (pDesktop)
            {
                ULONG chEaten;
                ULONG dwAttributes;
                LPITEMIDLIST pidl = NULL;

                HRESULT hr = pDesktop->ParseDisplayName(NULL, NULL, (LPWSTR)wInitialPath.c_str(), &chEaten, &pidl, &dwAttributes);
                if (SUCCEEDED(hr))
                {
                    bi.pidlRoot = pidl;
                }
                pDesktop->Release();
            }
        }

        // 显示目录选择对话框
        LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
        if (pidl != NULL)
        {
            // 获取选择的路径
            if (SHGetPathFromIDListW(pidl, path))
            {
                // 将宽字符串转换回 UTF-8
                int len = WideCharToMultiByte(CP_UTF8, 0, path, -1, NULL, 0, NULL, NULL);
                std::string result(len, 0);
                WideCharToMultiByte(CP_UTF8, 0, path, -1, &result[0], len, NULL, NULL);

                // 释放内存
                CoTaskMemFree(pidl);

                // 移除末尾的空字符
                if (!result.empty() && result.back() == '\0')
                {
                    result.pop_back();
                }

                return result;
            }
            CoTaskMemFree(pidl);
        }

        return std::string();
    }
}