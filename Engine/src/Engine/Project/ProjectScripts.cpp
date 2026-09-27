#include "ProjectScripts.h"
#include "Project.h"
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <vector>

namespace fs = std::filesystem;
namespace Engine
{
    namespace
    {
        struct Handle
        {
            HANDLE Value = INVALID_HANDLE_VALUE;
            ~Handle() { if (Value != INVALID_HANDLE_VALUE && Value != nullptr) CloseHandle(Value); }
        };

        // Windows argv quoting; no shell is used to execute project paths.
        std::wstring Quote(const std::wstring &value)
        {
            std::wstring result = L"\"";
            size_t slashes = 0;
            for (wchar_t c : value)
            {
                if (c == L'\\') { ++slashes; continue; }
                result.append(slashes * (c == L'"' ? 2 : 1), L'\\');
                slashes = 0;
                if (c == L'"') result += L'\\';
                result += c;
            }
            result.append(slashes * 2, L'\\');
            return result + L'"';
        }

        bool Run(const std::vector<std::wstring> &args, const fs::path &log)
        {
            std::wstring command;
            for (const auto &arg : args) command += Quote(arg) + L" ";
            SECURITY_ATTRIBUTES security{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
            Handle output{CreateFileW(log.c_str(), GENERIC_WRITE, FILE_SHARE_READ, &security,
                                       CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr)};
            Handle input{CreateFileW(L"NUL", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                     &security, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)};
            if (output.Value == INVALID_HANDLE_VALUE || input.Value == INVALID_HANDLE_VALUE) return false;
            STARTUPINFOW startup{};
            startup.cb = sizeof(startup);
            startup.dwFlags = STARTF_USESTDHANDLES;
            startup.hStdOutput = startup.hStdError = output.Value;
            startup.hStdInput = input.Value;
            PROCESS_INFORMATION process{};
            if (!CreateProcessW(nullptr, command.data(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW,
                                nullptr, nullptr, &startup, &process)) return false;
            Handle processHandle{process.hProcess}, threadHandle{process.hThread};
            WaitForSingleObject(process.hProcess, INFINITE);
            DWORD code = 1;
            return GetExitCodeProcess(process.hProcess, &code) && code == 0;
        }

        fs::path FailureMarker(const fs::path &assembly) { return fs::path(assembly.string() + ".build-failed"); }
    }

    fs::path ProjectScripts::AssemblyPath()
    {
        const auto &settings = Project::GetActiveProject()->GetProjectSettings();
        auto path = settings.ScriptAssembly;
        const std::string token = "{config}";
        size_t position = 0;
        while ((position = path.find(token, position)) != std::string::npos)
        {
            path.replace(position, token.size(), settings.ScriptBuildConfiguration);
            position += settings.ScriptBuildConfiguration.size();
        }
        return path.empty() ? fs::path{} : fs::absolute(Project::GetActiveProjectAssetPath(path)).lexically_normal();
    }

    fs::path ProjectScripts::SourceDirectory()
    {
        return fs::absolute(Project::GetActiveProjectAssetPath(
            Project::GetActiveProject()->GetProjectSettings().ScriptSourceDirectory)).lexically_normal();
    }

    bool ProjectScripts::CanLoad(const fs::path &assembly, std::string &error)
    {
        if (assembly.empty()) return true;
        if (fs::exists(FailureMarker(assembly)))
            error = "Script build failed or was interrupted. Rebuild before running: " + assembly.string();
        else if (!fs::is_regular_file(assembly))
            error = "Script assembly missing. Build project scripts first: " + assembly.string();
        else return true;
        return false;
    }

    bool ProjectScripts::Build(const fs::path &coreAssembly, std::string &error)
    {
        const auto &config = Project::GetActiveProject()->GetProjectSettings().ScriptBuildConfiguration;
        return Build(SourceDirectory(), AssemblyPath(), config, coreAssembly,
                     Project::GetActiveProjectDirectory() / "Intermediate/Scripts" / config, error);
    }

    bool ProjectScripts::Build(const fs::path &source, const fs::path &assembly,
                              const std::string &config, const fs::path &coreAssembly,
                              const fs::path &buildDirectory, std::string &error)
    {
        error.clear();
        try
        {
            if (assembly.empty()) return true;
            if (config != "Debug" && config != "Release")
                throw std::runtime_error("ScriptBuildConfiguration must be Debug or Release");
            const auto build = fs::absolute(buildDirectory);
            fs::create_directories(build);
            Handle lock{CreateFileW((build / "build.lock").c_str(), GENERIC_WRITE, 0, nullptr,
                                    OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr)};
            if (lock.Value == INVALID_HANDLE_VALUE)
                throw std::runtime_error("Another script build is running for this project");
            fs::create_directories(assembly.parent_path());
            std::ofstream marker(FailureMarker(assembly));
            if (!marker) throw std::runtime_error("Cannot write script build status");
            marker << "Build in progress or failed. See " << build.string();
            marker.close();
            if (!fs::is_regular_file(coreAssembly)) throw std::runtime_error("ScriptCore assembly is missing");
            if (!fs::is_directory(source)) throw std::runtime_error("Script source directory is missing");
            const fs::path cmake(FOREST_CMAKE_EXECUTABLE);
            const fs::path driver(FOREST_SCRIPT_BUILD_DRIVER);
            const auto configureLog = build / "configure.log";
            const auto buildLog = build / "build.log";
            auto failure = [&](const fs::path &log)
            {
                std::ifstream stream(log);
                std::ostringstream output;
                output << stream.rdbuf();
                error = "Script build failed. Log: " + log.string() + "\n" + output.str();
                ENGINE_ERROR("{}", error);
                return false;
            };
            ENGINE_INFO("Building project scripts: {} -> {}", source.string(), assembly.string());
            if (!Run({cmake.wstring(), L"-S", driver.wstring(), L"-B", build.wstring(),
                      L"-G", L"Visual Studio 17 2022", L"-A", L"x64",
                      L"-DFOREST_SCRIPT_SOURCE=" + fs::absolute(source).wstring(),
                      L"-DFOREST_SCRIPT_ASSEMBLY=" + assembly.wstring(),
                      L"-DFOREST_SCRIPT_CORE=" + fs::absolute(coreAssembly).wstring()}, configureLog))
                return failure(configureLog);
            if (!Run({cmake.wstring(), L"--build", build.wstring(), L"--config", fs::path(config).wstring(),
                      L"--target", L"GameScripts"}, buildLog)) return failure(buildLog);
            if (!fs::is_regular_file(assembly)) throw std::runtime_error("Build produced no script assembly");
            fs::remove(FailureMarker(assembly));
            ENGINE_INFO("Project scripts built: {}", assembly.string());
            return true;
        }
        catch (const std::exception &exception)
        {
            error = exception.what();
            ENGINE_ERROR("{}", error);
            return false;
        }
    }
}
