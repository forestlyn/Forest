#include "RuntimeLayer.h"
#include "Engine/Core/RuntimePaths.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/Log.h"
#include "Engine/Project/Project.h"
#include "Engine/Project/ProjectScripts.h"
#include "Engine/Project/ProjectExporter.h"
#include <charconv>
#include <iostream>
#include <stdexcept>
#include <yaml-cpp/yaml.h>

namespace fs = std::filesystem;

namespace ForestRuntime
{
    class RuntimeApp : public Engine::Core::Application
    {
    public:
        RuntimeApp(const Engine::Core::ApplicationSpecification &spec,
                   fs::path scenePath, uint32_t frameLimit)
            : Application(spec), m_ScenePath(std::move(scenePath)), m_FrameLimit(frameLimit) {}

        void Init() override
        {
            PushLayer(new RuntimeLayer(m_ScenePath, m_FrameLimit));
        }

    private:
        fs::path m_ScenePath;
        uint32_t m_FrameLimit;
    };

    void RequireFile(const fs::path &path)
    {
        if (!fs::is_regular_file(path))
            throw std::runtime_error("Required file not found: " + path.string());
    }

    fs::path ReadStartupProject()
    {
        const auto configPath = Engine::Core::RuntimePaths::ExecutableDirectory() / "runtime.yaml";
        if (!fs::is_regular_file(configPath))
            throw std::runtime_error("Startup config not found: " + configPath.string() +
                                     "; provide a project argument or create runtime.yaml with a Project entry");
        try
        {
            const auto config = YAML::LoadFile(configPath.string());
            if (!config.IsMap() || !config["Project"] || !config["Project"].IsScalar())
                throw std::runtime_error("Project must be a non-empty path string");
            const auto value = config["Project"].as<std::string>();
            if (value.empty() || value.find_first_not_of(" \t\r\n") == std::string::npos)
                throw std::runtime_error("Project must be a non-empty path string");
            return fs::absolute(configPath.parent_path() / fs::u8path(value)).lexically_normal();
        }
        catch (const std::exception &error)
        {
            throw std::runtime_error("Invalid startup config '" + configPath.string() + "': " + error.what());
        }
    }
}

int main(int argc, char **argv)
{
    Engine::Core::Log::Init();
    try
    {
        if (argc > 1 && std::string(argv[1]) == "--help")
        {
            std::cout << "Usage: ForestRuntime [project.forestproj] [--script-assembly <dll>] [--frames <count>] [--build-scripts | --build-only]\n"
                         "Without a project argument, read Project from runtime.yaml next to the executable.\n"
                         "Export: ForestRuntime <project.forestproj> --export <new-directory> [--runtime-directory <Release-runtime-directory>]\n"
                         "ScriptAssembly in the project is relative to Assets; the CLI DLL path is relative to the launch directory.\n";
            return 0;
        }

        const bool explicitProject = argc > 1 && std::string(argv[1]).rfind("--", 0) != 0;
        fs::path projectPath;
        if (explicitProject) projectPath = fs::absolute(argv[1]).lexically_normal();
        fs::path assemblyOverride;
        fs::path exportDirectory, exportRuntime;
        bool exportRequested = false;
        uint32_t frameLimit = 0;
        bool buildScripts = false, buildOnly = false;
        for (int i = explicitProject ? 2 : 1; i < argc; ++i)
        {
            const std::string option = argv[i];
            if (option == "--build-scripts" || option == "--build-only")
            {
                buildScripts = true;
                buildOnly |= option == "--build-only";
                continue;
            }
            if (option != "--script-assembly" && option != "--frames" && option != "--export" && option != "--runtime-directory")
                throw std::runtime_error("Unknown option: " + option);
            if (i + 1 >= argc)
                throw std::runtime_error("Missing value for " + option);
            const std::string value = argv[++i];
            if (value.empty()) throw std::runtime_error("Empty value for " + option);
            if (option == "--export")
            {
                exportRequested = true;
                exportDirectory = fs::absolute(value).lexically_normal();
            }
            else if (option == "--runtime-directory")
                exportRuntime = fs::absolute(value).lexically_normal();
            else if (option == "--script-assembly")
                assemblyOverride = fs::absolute(value).lexically_normal();
            else if (option == "--frames")
            {
                const auto result = std::from_chars(value.data(), value.data() + value.size(), frameLimit);
                if (result.ec != std::errc{} || result.ptr != value.data() + value.size() || !frameLimit)
                    throw std::runtime_error("--frames requires a positive integer");
            }
            else
                throw std::runtime_error("Unknown option: " + option);
        }

        if (exportRequested && (buildScripts || frameLimit || !assemblyOverride.empty()))
            throw std::runtime_error("--export cannot be combined with build or run options");
        if (!exportRequested && !exportRuntime.empty())
            throw std::runtime_error("--runtime-directory requires --export");
        if (!explicitProject) projectPath = ForestRuntime::ReadStartupProject();
        ForestRuntime::RequireFile(projectPath);
        if (projectPath.extension() != ".forestproj")
            throw std::runtime_error("Expected a .forestproj file");
        auto project = Engine::Project::Load(projectPath);
        if (!project)
            throw std::runtime_error("Failed to load project");
        if (exportRequested)
        {
            std::string error;
            if (!Engine::ProjectExporter::Export(exportDirectory, exportRuntime, error))
                throw std::runtime_error("Export failed: " + error);
            return 0;
        }
        const auto &settings = project->GetProjectSettings();
        const auto scenePath = Engine::Project::GetActiveProjectStartScene();
        if (!buildOnly) ForestRuntime::RequireFile(scenePath);

        const auto runtimeRoot = Engine::Core::RuntimePaths::ExecutableDirectory();
        Engine::Core::ApplicationSpecification spec;
        spec.Name = settings.Name;
        spec.EngineResourceDirectory = (runtimeRoot / "resources").string();
        spec.MonoAssemblyPath = (runtimeRoot / "Mono/4.5").string();
        spec.CoreAssemblyPath = (runtimeRoot / "Managed/Engine-ScriptCore.dll").string();
        Engine::Core::RuntimePaths::Configure(spec.EngineResourceDirectory);
        Engine::Core::Log::EnableFileLogging(Engine::Core::RuntimePaths::Logs() / "runtime.log");
        ENGINE_INFO("Runtime resources: {}", spec.EngineResourceDirectory);
        ENGINE_INFO("Runtime cache: {}", Engine::Core::RuntimePaths::Cache().string());
        spec.AppAssemblyPath.clear();
        if (!assemblyOverride.empty())
            spec.AppAssemblyPath = assemblyOverride.string();
        else
            spec.AppAssemblyPath = Engine::ProjectScripts::AssemblyPath().string();
        ForestRuntime::RequireFile(spec.CoreAssemblyPath);
        std::string scriptError;
        if (buildScripts)
        {
            if (!assemblyOverride.empty())
                throw std::runtime_error("Build uses project configuration; do not combine with --script-assembly");
            if (!Engine::ProjectScripts::Build(spec.CoreAssemblyPath, scriptError))
                throw std::runtime_error(scriptError);
        }
        if (buildOnly) return 0;
        if (!Engine::ProjectScripts::CanLoad(spec.AppAssemblyPath, scriptError))
            throw std::runtime_error(scriptError);
        ForestRuntime::RequireFile(fs::path(spec.MonoAssemblyPath) / "mscorlib.dll");
        if (!spec.AppAssemblyPath.empty())
            ForestRuntime::RequireFile(spec.AppAssemblyPath);
        for (const auto *shader : {"Renderer2D_QuadShader.glsl", "Renderer2D_CircleShader.glsl", "Renderer2D_LineShader.glsl"})
            ForestRuntime::RequireFile(fs::path(spec.EngineResourceDirectory) / "assets/shaders" / shader);
        spec.EnableImGui = false;
        spec.EnableProfileLayer = false;
        spec.EnableScriptDebugging = false;
        spec.EnableScriptHotReload = false;
        spec.RunInBackground = true;
        spec.CommandLineArgs = {argc, argv};
        ForestRuntime::RuntimeApp app(spec, scenePath, frameLimit);
        app.Init();
        app.Run();
        return 0;
    }
    catch (const std::exception &error)
    {
        ENGINE_ERROR("ForestRuntime: {}", error.what());
        return 1;
    }
}
