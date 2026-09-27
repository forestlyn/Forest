#include "Engine.h"
#include "Engine/Core/RuntimePaths.h"
#include "EngineEditor.h"
#include "Engine/Core/EntryPoint.h"

namespace EngineEditor
{
    class EngineEditorApp : public Engine::Core::Application
    {
    public:
        explicit EngineEditorApp(const Engine::Core::ApplicationSpecification &spec) : Application(spec) {}
        void Init() override { PushLayer(new EngineEditor()); }
    };
}

Engine::Core::Application *Engine::Core::CreateApplication(ApplicationCommandLineArgs args)
{
    ApplicationSpecification spec;
    const auto root = RuntimePaths::ExecutableDirectory();
    // Interpret CLI paths relative to the caller's working directory.
    static std::string projectPath;
    if (args.Count > 1)
    {
        projectPath = std::filesystem::absolute(args[1]).string();
        args.Args[1] = projectPath.data();
    }
    spec.Name = "Engine Editor";
    spec.Width = 1920;
    spec.Height = 1080;
    spec.EngineResourceDirectory = (root / "resources").string();
    spec.MonoAssemblyPath = (root / "Mono/4.5").string();
    spec.CoreAssemblyPath = (root / "Managed/Engine-ScriptCore.dll").string();
    spec.AppAssemblyPath.clear();
    spec.EnableScriptDebugging = false;
    spec.EnableScriptHotReload = false; // Source build and reload are coordinated by the editor layer.
    spec.CommandLineArgs = args;
    return new EngineEditor::EngineEditorApp(spec);
}
