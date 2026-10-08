#include "Panels/SceneHierarchyPanel.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/RuntimePaths.h"
#include "Engine/Core/Log.h"
#include "Engine/Project/Project.h"
#include "Engine/Serialization/SceneSerialize.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <filesystem>
#include <fstream>
#include <stdexcept>

using namespace Engine;
namespace EngineEditor
{
    struct HierarchyPanelTestAccess
    {
        using Action = SceneHierarchyPanel::Action;
        static void Queue(SceneHierarchyPanel &panel, Action action, UUID source = UUID(0), UUID target = UUID(0))
        { panel.m_PendingActions.push_back({action, source, target}); }
        static void Apply(SceneHierarchyPanel &panel) { panel.ApplyPendingActions(); }
        static bool HasError(const SceneHierarchyPanel &panel) { return !panel.m_HierarchyMessage.empty(); }
        static bool Expanded(const SceneHierarchyPanel &panel, UUID id) { return panel.m_Expanded.contains(id); }
        static uint64_t SelectionID(const SceneHierarchyPanel &panel) { return uint64_t(panel.m_SelectionEntityID); }
    };
}
using Access = EngineEditor::HierarchyPanelTestAccess;
using Action = Access::Action;
void Check(bool condition, const char *message) { if (!condition) throw std::runtime_error(message); }

void Frame(EngineEditor::SceneHierarchyPanel &panel, ImVec2 mouse = {-100, -100}, int down = -1)
{
    auto &io = ImGui::GetIO();
    io.AddMousePosEvent(mouse.x, mouse.y);
    if (down >= 0) io.AddMouseButtonEvent(0, down != 0);
    ImGui::NewFrame();
    ImGui::SetNextWindowPos({550, 20}, ImGuiCond_Always);
    ImGui::SetNextWindowSize({400, 650}, ImGuiCond_Always);
    ImGui::Begin("Properties"); ImGui::End();
    ImGui::SetNextWindowPos({20, 20}, ImGuiCond_Always);
    ImGui::SetNextWindowSize({450, 650}, ImGuiCond_Always);
    panel.OnImGuiRender();
    ImGui::Render();
}

int main(int argc, char **argv)
{
    Core::Log::Init();
    try
    {
        Check(argc == 2, "output directory required");
        const auto output = std::filesystem::absolute(argv[1]);
        std::filesystem::create_directories(output / "Assets");
        std::ofstream(output / "Test.forestproj") << "Name: HierarchyTests\nAssetDirectory: Assets\nScriptAssembly: ''\n";
        Check(bool(Project::Load(output / "Test.forestproj")), "project setup");
        const auto root = Core::RuntimePaths::ExecutableDirectory();
        Core::ApplicationSpecification spec;
        spec.Name = "Hierarchy tests"; spec.Width = spec.Height = 64;
        spec.WindowVisible = false; spec.VSync = false;
        spec.EnableImGui = spec.EnableProfileLayer = spec.EnableScriptDebugging = spec.EnableScriptHotReload = false;
        spec.EngineResourceDirectory = (root / "resources").string();
        spec.UserDataDirectory = (output / "UserData").string();
        spec.MonoAssemblyPath = (root / "Mono/4.5").string();
        spec.CoreAssemblyPath = (root / "Managed/Engine-ScriptCore.dll").string();
        spec.AppAssemblyPath.clear();
        Core::Application app(spec);
        ImGui::CreateContext();
        auto &io = ImGui::GetIO();
        io.IniFilename = nullptr; io.DisplaySize = {1000, 720}; io.DeltaTime = 1.0f / 60;
        unsigned char *pixels; int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        io.Fonts->SetTexID(ImTextureID(1));
        {
            auto scene = CreateRef<Scene>();
            auto a = scene->CreateEntity("A");
            auto b = scene->CreateEntity("B"); b.SetParent(a);
            auto c = scene->CreateEntity("C"); c.SetParent(b);
            auto d = scene->CreateEntity("D"); d.SetSiblingIndex(UINT32_MAX);
            EngineEditor::SceneHierarchyPanel panel(scene);
            panel.SetSelectedEntity(c);
            Check(Access::Expanded(panel, a.GetUUID()) && Access::Expanded(panel, b.GetUUID()), "reveal ancestors");
            Frame(panel); Frame(panel);
            const auto *window = ImGui::FindWindowByName("Scene Hierarchy");
            const float line = ImGui::GetFontSize() + ImGui::GetStyle().ItemSpacing.y;
            const ImVec2 first = {window->DC.CursorStartPos.x + 90, window->DC.CursorStartPos.y + line * 0.5f};
            const ImVec2 fourth = {first.x, first.y + line * 3};
            Frame(panel, fourth, 1); Frame(panel, fourth, 0);
            Check(panel.GetSelectedEntity() == d, "real tree row click selects D after nested subtree");
            // Drag D onto A using the actual source/target handlers.
            Frame(panel, fourth, 1);
            Frame(panel, {fourth.x + 20, fourth.y}, 1);
            Check(ImGui::GetDragDropPayload() != nullptr, "entity drag payload created");
            Check(ImGui::GetDragDropPayload()->DataSize == sizeof(UUID), "legacy UUID payload retained");
            Frame(panel, first, 1); Frame(panel, first, 1);
            Frame(panel, first, 0); Frame(panel, first);
            Check(d.GetParent() == a && a.GetChildren().back() == d, "actual drop reparents and appends");
            Check(panel.GetSelectedEntity() == d, "drop keeps selection");
            const ImVec2 arrow = {window->DC.CursorStartPos.x + 5, first.y};
            Frame(panel, arrow, 1); Frame(panel, arrow, 0);
            Check(!Access::Expanded(panel, a.GetUUID()) && panel.GetSelectedEntity() == d, "collapse arrow preserves selected descendant");
            Frame(panel, arrow, 1); Frame(panel, arrow, 0);
            Check(Access::Expanded(panel, a.GetUUID()), "expand arrow reopens tree");
            Frame(panel, {400, 600}, 1); Frame(panel, {400, 600}, 0);
            Check(!panel.GetSelectedEntity(), "only blank click clears selection");
            Frame(panel, fourth, 1); Frame(panel, {fourth.x + 20, fourth.y}, 1);
            Check(ImGui::GetDragDropPayload() != nullptr, "second drag starts");
            auto playScene = Scene::Copy(scene);
            panel.SetContext(playScene);
            Check(!ImGui::GetDragDropPayload(), "scene switch cancels legacy drag before reference fields can accept it");
            Frame(panel, first, 0);
            Check(playScene->GetEntityByUUID(d.GetUUID()).GetParent().GetUUID() == a.GetUUID(), "stale drag cannot change copied scene");
            panel.SetContext(scene);

            Access::Queue(panel, Action::Reparent, a.GetUUID(), c.GetUUID()); Access::Apply(panel);
            Check(!a.GetParent() && Access::HasError(panel), "cycle rejected visibly");
            Access::Queue(panel, Action::MoveUp, d.GetUUID()); Access::Apply(panel);
            Check(a.GetChildren().front() == d, "move up");
            Access::Queue(panel, Action::Duplicate, b.GetUUID()); Access::Apply(panel);
            auto copy = panel.GetSelectedEntity();
            Check(copy != b && copy.GetChildren().size() == 1 && copy.GetSiblingIndex() == b.GetSiblingIndex() + 1, "copy subtree inserted after original");
            Access::Queue(panel, Action::CreateChild, copy.GetUUID()); Access::Apply(panel);
            Check(panel.GetSelectedEntity().GetParent() == copy && panel.GetSelectedEntity().GetSiblingIndex() == 1, "create child appends");
            auto path = output / "tree.scene";
            Serialization::SceneSerialize(scene).Serialize(path.string());
            auto loaded = CreateRef<Scene>();
            Check(Serialization::SceneSerialize(loaded).Deserialize(path.string()), "save reload");
            Check(loaded->GetEntityByUUID(a.GetUUID()).GetChildren()[0].GetUUID() == d.GetUUID(), "ordering persists");
            panel.SetSelectedEntity(c);
            Access::Queue(panel, Action::Delete, b.GetUUID()); Access::Apply(panel);
            Check(!panel.GetSelectedEntity() && Access::SelectionID(panel) == 0, "deleting parent clears selected descendant immediately");
            scene->FlushPendingEntityDestruction(); Frame(panel);
            Access::Queue(panel, Action::Detach, d.GetUUID()); Access::Apply(panel);
            Check(!d.GetParent() && scene->GetRootEntities().back() == d, "detach appends root");
            Access::Queue(panel, Action::CreateCanvas); Access::Apply(panel);
            auto canvas = panel.GetSelectedEntity();
            Access::Queue(panel, Action::CreateChild, canvas.GetUUID()); Access::Apply(panel);
            auto ui = panel.GetSelectedEntity();
            Check(ui.HasComponent<RectTransformComponent>() && !ui.HasComponent<TransformComponent>() && !canvas.HasComponent<TransformComponent>() && ui.GetParent() == canvas, "UI child creation");
            Access::Queue(panel, Action::Reparent, ui.GetUUID(), a.GetUUID()); Access::Apply(panel);
            Check(ui.GetParent() == canvas && Access::HasError(panel), "UI cannot attach to world entity");
            Access::Queue(panel, Action::Detach, ui.GetUUID()); Access::Apply(panel);
            Check(!ui.GetParent() && Access::HasError(panel), "detached UI has visible notice");
            panel.SetSelectedEntity(a);
            Access::Queue(panel, Action::Delete, a.GetUUID());
            panel.SetContext(loaded); Access::Apply(panel);
            Check(panel.GetSelectedEntity() == loaded->GetEntityByUUID(a.GetUUID()), "context switch restores UUID in new scene and cancels queued edit");
            Check(!loaded->IsPendingDestruction(panel.GetSelectedEntity()), "old scene command not applied to copied UUID");
            panel.SetContext(nullptr); Frame(panel);
            Check(!panel.GetSelectedEntity() && Access::SelectionID(panel) == 0, "null context clears selection");
            panel.SetContext(scene);
            auto bare = scene->CreateEntityWithID(UUID(42));
            panel.SetSelectedEntity(bare); Frame(panel);
            bare.Destroy(); scene->FlushPendingEntityDestruction(); Frame(panel);
            Check(!panel.GetSelectedEntity() && Access::SelectionID(panel) == 0, "external deletion and tagless row safe");
        }
        ImGui::DestroyContext();
        app.FlushRendererCommands();
        ENGINE_INFO("PASS: hierarchy ImGui selection/drop, deferred edits, subtree operations, persistence and context safety");
        return 0;
    }
    catch (const std::exception &error)
    {
        ENGINE_ERROR("Hierarchy panel test failed: {}", error.what());
        return 1;
    }
}
