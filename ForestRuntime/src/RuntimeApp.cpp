#include "RuntimeLayer.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/Log.h"
#include "Engine/Project/Project.h"
#include <charconv>
#include <iostream>
#include <stdexcept>

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
}

int main(int argc, char **argv)
{
    Engine::Core::Log::Init();
    try
    {
        if (argc < 2 || std::string(argv[1]) == "--help")
        {
            std::cout << "Usage: ForestRuntime <project.forestproj> [--script-assembly <dll>] [--frames <count>]\n"
                         "ScriptAssembly in the project is relative to Assets; the CLI DLL path is relative to the launch directory.\n";
            return argc < 2 ? 1 : 0;
        }

        const fs::path projectPath = fs::absolute(argv[1]).lexically_normal();
        fs::path assemblyOverride;
        uint32_t frameLimit = 0;
        for (int i = 2; i < argc; ++i)
        {
            const std::string option = argv[i];
            if (i + 1 >= argc)
                throw std::runtime_error("Missing value for " + option);
            const std::string value = argv[++i];
            if (option == "--script-assembly")
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

        ForestRuntime::RequireFile(projectPath);
        if (projectPath.extension() != ".forestproj")
            throw std::runtime_error("Expected a .forestproj file");
        auto project = Engine::Project::Load(projectPath);
        if (!project)
            throw std::runtime_error("Failed to load project");
        const auto &settings = project->GetProjectSettings();
        const auto scenePath = Engine::Project::GetActiveProjectStartScene();
        ForestRuntime::RequireFile(scenePath);

        const fs::path sourceRoot(FOREST_SOURCE_ROOT);
        Engine::Core::ApplicationSpecification spec;
        spec.Name = settings.Name;
        // Development-only resource location. Exporting a relocatable package is a separate step.
        spec.WorkingDirectory = (sourceRoot / "EngineEditor").string();
        spec.MonoAssemblyPath = (sourceRoot / "Engine/ThirdParty/mono/4.5").string();
        spec.CoreAssemblyPath = (sourceRoot / "EngineEditor/resources/scripts/bin/Engine-ScriptCore.dll").string();
        spec.AppAssemblyPath.clear();
        if (!assemblyOverride.empty())
            spec.AppAssemblyPath = assemblyOverride.string();
        else if (!settings.ScriptAssembly.empty())
            spec.AppAssemblyPath = Engine::Project::GetActiveProjectAssetPath(settings.ScriptAssembly).string();
        ForestRuntime::RequireFile(spec.CoreAssemblyPath);
        ForestRuntime::RequireFile(fs::path(spec.MonoAssemblyPath) / "mscorlib.dll");
        if (!spec.AppAssemblyPath.empty())
            ForestRuntime::RequireFile(spec.AppAssemblyPath);
        for (const auto *shader : {"Renderer2D_QuadShader.glsl", "Renderer2D_CircleShader.glsl", "Renderer2D_LineShader.glsl"})
            ForestRuntime::RequireFile(fs::path(spec.WorkingDirectory) / "resources/assets/shaders" / shader);
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
