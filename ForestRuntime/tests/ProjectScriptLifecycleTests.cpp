#include "Engine/Core/Application.h"
#include "Engine/Project/Project.h"
#include "Engine/Project/ProjectScripts.h"
#include "Engine/Scripts/ScriptEngine.h"
#include "Engine/Serialization/SceneSerialize.h"
#include <stdexcept>

namespace fs = std::filesystem;
int main(int argc, char **argv)
{
    Engine::Core::Log::Init();
    try
    {
        if (argc != 3) throw std::runtime_error("Expected two project paths");
        const fs::path first = fs::absolute(argv[1]), second = fs::absolute(argv[2]);
        const fs::path root(FOREST_SOURCE_ROOT);
        Engine::Core::ApplicationSpecification spec;
        spec.WorkingDirectory = (root / "EngineEditor").string();
        spec.MonoAssemblyPath = (root / "Engine/ThirdParty/mono/4.5").string();
        spec.CoreAssemblyPath = (root / "EngineEditor/resources/scripts/bin/Engine-ScriptCore.dll").string();
        spec.AppAssemblyPath.clear();
        spec.EnableImGui = spec.EnableProfileLayer = spec.EnableScriptDebugging = spec.EnableScriptHotReload = false;
        Engine::Core::Application app(spec);
        std::string previous;
        for (const auto &path : {first, second, first})
        {
            auto project = Engine::Project::Load(path);
            if (!project) throw std::runtime_error("Project load failed");
            Engine::ScriptEngine::LoadProjectAssembly(Engine::ProjectScripts::AssemblyPath(), true);
            const auto name = project->GetProjectSettings().Name + ".Player";
            if (!Engine::ScriptEngine::EntityClassExists(name)) throw std::runtime_error("New script type missing");
            if (!previous.empty() && Engine::ScriptEngine::EntityClassExists(previous))
                throw std::runtime_error("Previous project type leaked");
            auto scene = Engine::CreateRef<Engine::Scene>();
            Engine::Serialization::SceneSerialize serializer(scene);
            if (!serializer.Deserialize(Engine::Project::GetActiveProjectStartScene().string()))
                throw std::runtime_error("Scene load failed");
            scene->SetViewportSize(1280, 720);
            const auto entities = scene->GetRegistry().view<Engine::IDComponent, Engine::ScriptComponent>();
            for (auto entity : entities)
            {
                auto &value = Engine::ScriptEngine::GetScriptFieldMap(
                    scene->GetRegistry().get<Engine::IDComponent>(entity).ID)["Value"];
                value.Field = Engine::ScriptEngine::GetEntityClass(name)->GetFields().at("Value");
                value.SetValue<int>(77);
            }
            // Simulate Ctrl+R/Play: preserve scene field values while rebinding metadata.
            Engine::ScriptEngine::LoadProjectAssembly(Engine::ProjectScripts::AssemblyPath());
            Engine::ScriptEngine::RefreshSceneFields(scene.get());
            scene->OnRuntimeStart();
            for (auto entity : entities)
            {
                auto instance = Engine::ScriptEngine::GetEntityScriptInstance(
                    scene->GetRegistry().get<Engine::IDComponent>(entity).ID);
                if (!instance || instance->GetFieldValue<int>("Value") != 77)
                    throw std::runtime_error("Scene field value lost after script reload");
            }
            scene->OnUpdateRuntime(0.016f);
            app.FlushRendererCommands();
            scene->OnRuntimeStop();
            if (Engine::ScriptEngine::GetSceneContext()) throw std::runtime_error("Stopped scene still active");
            // This is also the failed-build policy used by the editor.
            Engine::ScriptEngine::LoadProjectAssembly({});
            if (Engine::ScriptEngine::EntityClassExists(name)) throw std::runtime_error("Old DLL still active after unload");
            previous = name;
        }
        ENGINE_INFO("PASS: project switch, scene metadata rebind, script instance creation and unload");
        return 0;
    }
    catch (const std::exception &error)
    {
        ENGINE_ERROR("{}", error.what());
        return 1;
    }
}
