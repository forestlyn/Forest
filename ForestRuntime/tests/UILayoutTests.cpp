#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"
#include "Engine/UI/UILayout.h"
#include "Engine/Serialization/SceneSerialize.h"
#include "Engine/Serialization/ComponentSerialize.h"
#include "Engine/Core/Log.h"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <limits>
#include <stdexcept>

using namespace Engine;

namespace
{
    void Check(bool value, const char *message) { if (!value) throw std::runtime_error(message); }
    bool Near(float a, float b) { return std::abs(a - b) < 0.002f; }
    bool Near(glm::vec2 a, glm::vec2 b) { return Near(a.x, b.x) && Near(a.y, b.y); }
    const UI::LayoutRect &Find(const UI::Layout &layout, UUID id)
    {
        for (const auto &rect : layout.Rects) if (rect.EntityID == id) return rect;
        throw std::runtime_error("Expected layout rectangle missing");
    }

    Entity Fixed(Scene &scene, uint64_t id, bool canvas = false, uint64_t parent = 0)
    {
        auto entity = scene.CreateEntityWithID(UUID(id));
        entity.AddComponent<TagComponent>("UI");
        entity.AddComponent<TransformComponent>();
        entity.AddComponent<RectTransformComponent>().Parent.uuid = UUID(parent);
        if (canvas) entity.AddComponent<CanvasComponent>();
        return entity;
    }

    void LayoutAndScaling()
    {
        Scene scene;
        auto canvas = scene.CreateCanvas();
        auto panel = scene.CreateUIEntity(canvas, "Panel");
        auto label = scene.CreateUIEntity(panel, "Label");
        panel.GetComponent<RectTransformComponent>().SizeDelta = {200, 100};
        label.GetComponent<RectTransformComponent>().SizeDelta = {40, 20};
        auto layout = UI::CalculateLayout(scene.GetRegistry(), {1280, 720});
        Check(layout.Diagnostics.empty() && layout.Rects.size() == 3, "basic layout");
        Check(Near(Find(layout, panel.GetUUID()).Min, {540, 310}), "center anchor");
        Check(Near(Find(layout, label.GetUUID()).Min, {620, 350}), "nested center anchor");
        panel.GetComponent<RectTransformComponent>().AnchoredPosition = {30, -10};
        layout = UI::CalculateLayout(scene.GetRegistry(), {1280, 720});
        Check(Near(Find(layout, label.GetUUID()).Min, {650, 340}), "child follows parent");
        auto &rect = panel.GetComponent<RectTransformComponent>();
        rect.AnchorMin = {0, 0}; rect.AnchorMax = {1, 1};
        rect.Pivot = {0.5f, 0.5f}; rect.AnchoredPosition = {0, 0}; rect.SizeDelta = {-20, -40};
        layout = UI::CalculateLayout(scene.GetRegistry(), {1280, 720});
        Check(Near(Find(layout, panel.GetUUID()).Min, {10, 20}) &&
              Near(Find(layout, panel.GetUUID()).Size, {1260, 680}), "stretched margins");
        auto &settings = canvas.GetComponent<CanvasComponent>();
        for (float match : {0.0f, 0.5f, 1.0f})
        {
            settings.MatchWidthOrHeight = match;
            layout = UI::CalculateLayout(scene.GetRegistry(), {2560, 720});
            float expected = match == 0 ? 2.0f : match == 1 ? 1.0f : std::sqrt(2.0f);
            Check(Near(layout.Rects.front().CanvasScale, expected), "width/height match scaling");
        }
        settings.ScaleMode = CanvasScaleMode::ConstantPixelSize;
        layout = UI::CalculateLayout(scene.GetRegistry(), {1920, 1080});
        Check(Near(layout.Rects.front().Size, {1920, 1080}) && Near(layout.Rects.front().CanvasScale, 1), "pixel scale");
        rect.SetEnabled(false);
        layout = UI::CalculateLayout(scene.GetRegistry(), {1280, 720});
        Check(!Find(layout, label.GetUUID()).Enabled && Find(layout, canvas.GetUUID()).Enabled, "disabled subtree");
        rect.SetEnabled(true); settings.SetEnabled(false);
        layout = UI::CalculateLayout(scene.GetRegistry(), {1280, 720});
        for (const auto &entry : layout.Rects) Check(!entry.Enabled, "disabled canvas");
        Check(UI::CalculateLayout(scene.GetRegistry(), {0, 720}).Rects.empty(), "zero viewport");
        Check(UI::CalculateLayout(scene.GetRegistry(), {-1, 720}).Rects.empty(), "negative viewport");
        settings.ReferenceResolution.x = 0;
        layout = UI::CalculateLayout(scene.GetRegistry(), {1280, 720});
        Check(layout.Rects.empty() && !layout.Diagnostics.empty(), "invalid canvas quarantined");
        settings.ReferenceResolution.x = 1280;
        rect.SizeDelta.x = -2000;
        layout = UI::CalculateLayout(scene.GetRegistry(), {1280, 720});
        Check(layout.Rects.size() == 1 && !layout.Diagnostics.empty(), "negative child size quarantines descendants");
        rect.SizeDelta.x = 0; rect.AnchoredPosition.x = std::numeric_limits<float>::infinity();
        Check(!UI::CalculateLayout(scene.GetRegistry(), {1280, 720}).Diagnostics.empty(), "non-finite offset");
    }

    void HierarchyAndOrdering()
    {
        Scene scene;
        auto root = Fixed(scene, 100, true);
        auto a = Fixed(scene, 10, false, 100);
        auto b = Fixed(scene, 20, false, 100);
        auto child = Fixed(scene, 5, false, 10);
        auto other = Fixed(scene, 200, true);
        other.GetComponent<CanvasComponent>().SortOrder = -1;
        auto hierarchy = UI::BuildHierarchy(scene.GetRegistry());
        Check(hierarchy.Diagnostics.empty(), "valid hierarchy");
        const uint64_t expected[]{200, 100, 10, 5, 20};
        for (size_t i = 0; i < 5; ++i) Check(uint64_t(hierarchy.Nodes[i].ID) == expected[i], "stable depth-first order");
        Check(scene.SetUISiblingOrder(b, -1), "set sibling order");
        hierarchy = UI::BuildHierarchy(scene.GetRegistry());
        Check(hierarchy.Nodes[2].ID == b.GetUUID(), "sibling reorder");
        Check(!scene.SetUIParent(a, a), "reject self parent");
        Check(!scene.SetUIParent(a, child), "reject cycle");
        Check(!scene.SetUIParent(root, other), "reject nested canvas");
        Scene foreign;
        auto foreignCanvas = foreign.CreateCanvas();
        Check(!scene.SetUIParent(a, foreignCanvas), "reject cross-scene parent");
        auto world = scene.CreateEntity("World");
        Check(!scene.CreateUIEntity(world), "reject non-UI parent");
        Check(scene.SetUIParent(a, other), "valid reparent across root canvases");
        hierarchy = UI::BuildHierarchy(scene.GetRegistry());
        Check(hierarchy.Nodes[2].Canvas == other.GetUUID(), "descendant changes canvas");
        // Simulate corrupt serialized fields that bypass the checked mutation API.
        a.GetComponent<RectTransformComponent>().Parent.uuid = child.GetUUID();
        hierarchy = UI::BuildHierarchy(scene.GetRegistry());
        Check(hierarchy.Nodes.size() == 3 && hierarchy.Diagnostics.size() == 2, "cycle quarantined");
        a.GetComponent<RectTransformComponent>().Parent.uuid = UUID(999);
        Check(UI::BuildHierarchy(scene.GetRegistry()).Diagnostics.size() == 2, "missing parent quarantines subtree");
        a.GetComponent<RectTransformComponent>().Parent.uuid = root.GetUUID();
        a.AddComponent<CanvasComponent>();
        Check(UI::BuildHierarchy(scene.GetRegistry()).Diagnostics.size() == 2, "nested canvas quarantines subtree");
        a.RemoveComponent<CanvasComponent>();
        auto brokenCanvas = scene.CreateEntity("BrokenCanvas");
        brokenCanvas.AddComponent<CanvasComponent>();
        Check(!UI::BuildHierarchy(scene.GetRegistry()).Diagnostics.empty(), "canvas requires rect");
        scene.DestroyEntity(b);
        Check(!scene.CreateUIEntity(b), "cannot parent to pending deletion");
    }

    void CopyAndDelete()
    {
        auto scene = CreateRef<Scene>();
        auto canvas = scene->CreateCanvas();
        auto panel = scene->CreateUIEntity(canvas, "Panel");
        auto child = scene->CreateUIEntity(panel, "Child");
        auto sibling = scene->CreateUIEntity(canvas, "Sibling");
        panel.GetComponent<RectTransformComponent>().SetEnabled(false);
        const UUID canvasID = canvas.GetUUID(), panelID = panel.GetUUID(), childID = child.GetUUID();
        auto copy = scene->DuplicateUISubtree(panel);
        Check(copy && copy.GetUUID() != panelID, "new subtree UUID");
        Check(copy.GetComponent<RectTransformComponent>().Parent.uuid == canvasID, "external parent preserved");
        auto copiedChild = scene->FindEntityByName("Child_Copy");
        Check(copiedChild && copiedChild.GetComponent<RectTransformComponent>().Parent.uuid == copy.GetUUID(), "internal parent remapped");
        Check(!copy.GetComponent<RectTransformComponent>().IsEnabled(), "enabled copied");
        auto copiedScene = Scene::Copy(scene);
        Check(copiedScene->GetEntityByUUID(childID).GetComponent<RectTransformComponent>().Parent.uuid == panelID,
              "scene copy preserves UUID relationships");
        auto fullRootCopy = scene->DuplicateUISubtree(canvas);
        Check(fullRootCopy && uint64_t(fullRootCopy.GetComponent<RectTransformComponent>().Parent.uuid) == 0, "canvas copy stays root");
        auto hierarchy = UI::BuildHierarchy(scene->GetRegistry());
        Check(hierarchy.Diagnostics.empty(), "copied hierarchy valid");
        // The editor still uses TagComponent directly; it must get cascade semantics too.
        panel.GetComponent<TagComponent>().SetRemove(true);
        scene->FlushPendingEntityDestruction();
        Check(!scene->GetEntityByUUID(panelID) && !scene->GetEntityByUUID(childID), "cascade deletion");
        Check(scene->GetEntityByUUID(sibling.GetUUID()) && scene->GetEntityByUUID(copy.GetUUID()), "unrelated branches survive");
        Check(copiedScene->GetEntityByUUID(panelID) && copiedScene->GetEntityByUUID(childID), "scene copies isolated");
        Check(!scene->SetUISiblingOrder(child, 1), "stale entity rejected");
        scene->DestroyEntity(canvas);
        scene->FlushPendingEntityDestruction();
        Check(UI::BuildHierarchy(scene->GetRegistry()).Diagnostics.empty(), "no orphans after deleting canvas");
        Check(scene->GetEntityByUUID(fullRootCopy.GetUUID()), "other canvas survives");
    }

    void PersistenceTests(const std::filesystem::path &directory)
    {
        auto scene = CreateRef<Scene>();
        // Parent UUID sorts after children, exercising forward references on load.
        auto canvas = Fixed(*scene, 300, true);
        auto panel = Fixed(*scene, 200, false, 300);
        auto child = Fixed(*scene, 100, false, 200);
        canvas.GetComponent<CanvasComponent>().SortOrder = 12;
        canvas.GetComponent<CanvasComponent>().MatchWidthOrHeight = 0.25f;
        canvas.GetComponent<CanvasComponent>().SetEnabled(false);
        panel.GetComponent<RectTransformComponent>().SiblingOrder = -3;
        panel.GetComponent<RectTransformComponent>().AnchoredPosition = {13, 27};
        panel.GetComponent<RectTransformComponent>().SetEnabled(false);
        const auto file = directory / "ui-p1.scene";
        Serialization::SceneSerialize(scene).Serialize(file.string());
        auto loaded = CreateRef<Scene>();
        Check(Serialization::SceneSerialize(loaded).Deserialize(file.string()), "round trip load");
        Check(UI::BuildHierarchy(loaded->GetRegistry()).Diagnostics.empty(), "forward parent references resolved");
        auto &c = loaded->GetEntityByUUID(UUID(300)).GetComponent<CanvasComponent>();
        auto &r = loaded->GetEntityByUUID(UUID(200)).GetComponent<RectTransformComponent>();
        Check(!c.IsEnabled() && !r.IsEnabled(), "inherited enabled persisted");
        Check(c.SortOrder == 12 && Near(c.MatchWidthOrHeight, 0.25f) && r.SiblingOrder == -3 &&
              Near(r.AnchoredPosition, {13, 27}), "component values persisted");
        const auto again = directory / "ui-p1-again.scene";
        Serialization::SceneSerialize(loaded).Serialize(again.string());
        Check(YAML::Dump(YAML::LoadFile(file.string())) == YAML::Dump(YAML::LoadFile(again.string())), "stable serialization");
        Check(!YAML::LoadFile(file.string())["Entities"][0]["RectTransformComponent"]["ComputedRect"], "no layout cache saved");
        auto legacyFile = directory / "legacy.scene";
        std::ofstream(legacyFile) << "Scene: Legacy\nEntities:\n  - EntityID: 1\n    TagComponent: {Tag: Old}\n    TransformComponent: {}\n";
        auto legacy = CreateRef<Scene>();
        Check(Serialization::SceneSerialize(legacy).Deserialize(legacyFile.string()), "old scene loads");
        Check(UI::BuildHierarchy(legacy->GetRegistry()).Nodes.empty(), "old scene unchanged");
        auto minimalFile = directory / "defaults.scene";
        std::ofstream(minimalFile) << "Scene: Defaults\nEntities:\n  - EntityID: 1\n    TagComponent: {Tag: Canvas}\n    CanvasComponent: {}\n    RectTransformComponent: {}\n";
        auto minimal = CreateRef<Scene>();
        Check(Serialization::SceneSerialize(minimal).Deserialize(minimalFile.string()), "missing fields use defaults");
        auto layout = UI::CalculateLayout(minimal->GetRegistry(), {1280, 720});
        Check(layout.Diagnostics.empty() && layout.Rects.size() == 1 && layout.Rects[0].Enabled, "default layout valid");
    }
}

int main(int argc, char **argv)
{
    Core::Log::Init();
    try
    {
        Check(argc == 2, "Expected test output directory");
        const auto directory = std::filesystem::absolute(argv[1]);
        std::filesystem::create_directories(directory);
        LayoutAndScaling();
        HierarchyAndOrdering();
        CopyAndDelete();
        PersistenceTests(directory);
        ENGINE_INFO("PASS: UI layout, scaling, hierarchy, ordering, copy, deletion and scene persistence");
        return 0;
    }
    catch (const std::exception &error)
    {
        ENGINE_ERROR("UI P1 test failed: {}", error.what());
        return 1;
    }
}
