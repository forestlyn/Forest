#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/ScriptEntity.h"
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
    void Check(bool value, const char *message)
    {
        if (!value)
            throw std::runtime_error(message);
    }
    bool Near(float a, float b) { return std::abs(a - b) < 0.002f; }
    bool Near(glm::vec2 a, glm::vec2 b) { return Near(a.x, b.x) && Near(a.y, b.y); }
    const UI::LayoutRect &Find(const UI::Layout &layout, UUID id)
    {
        for (const auto &rect : layout.Rects)
            if (rect.EntityID == id)
                return rect;
        throw std::runtime_error("Expected layout rectangle missing");
    }

    // Deliberately bypass Scene only to exercise layout diagnostics on corrupt imported data.
    void ForceParent(Scene &scene, Entity entity, UUID parent)
    {
        YAML::Node node;
        node["Parent"] = uint64_t(parent);
        RelationshipComponent relation;
        Serialization::DeserializeComponent(node, relation);
        scene.GetRegistry().replace<RelationshipComponent>(entity, relation);
    }

    Entity Fixed(Scene &scene, uint64_t id, bool canvas = false, uint64_t parent = 0)
    {
        auto entity = scene.CreateEntityWithID(UUID(id));
        entity.AddComponent<TagComponent>("UI");
        entity.AddComponent<RectTransformComponent>();
        if (parent) Check(scene.SetParent(entity, scene.GetEntityByUUID(UUID(parent))), "fixed entity parent");
        if (canvas)
            entity.AddComponent<CanvasComponent>();
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
        rect.AnchorMin = {0, 0};
        rect.AnchorMax = {1, 1};
        rect.Pivot = {0.5f, 0.5f};
        rect.AnchoredPosition = {0, 0};
        rect.SizeDelta = {-20, -40};
        layout = UI::CalculateLayout(scene.GetRegistry(), {1280, 720});
        Check(Near(Find(layout, panel.GetUUID()).Min, {10, 20}) &&
                  Near(Find(layout, panel.GetUUID()).Size, {1260, 680}),
              "stretched margins");
        auto &settings = canvas.GetComponent<CanvasComponent>();
        for (float match : {0.0f, 0.5f, 1.0f})
        {
            settings.MatchWidthOrHeight = match;
            layout = UI::CalculateLayout(scene.GetRegistry(), {2560, 720});
            float expected = match == 0 ? 2.0f : match == 1 ? 1.0f
                                                            : std::sqrt(2.0f);
            Check(Near(layout.Rects.front().CanvasScale, expected), "width/height match scaling");
        }
        settings.ScaleMode = CanvasScaleMode::ConstantPixelSize;
        layout = UI::CalculateLayout(scene.GetRegistry(), {1920, 1080});
        Check(Near(layout.Rects.front().Size, {1920, 1080}) && Near(layout.Rects.front().CanvasScale, 1), "pixel scale");
        rect.SetEnabled(false);
        layout = UI::CalculateLayout(scene.GetRegistry(), {1280, 720});
        Check(!Find(layout, label.GetUUID()).Enabled && Find(layout, canvas.GetUUID()).Enabled, "disabled subtree");
        rect.SetEnabled(true);
        settings.SetEnabled(false);
        layout = UI::CalculateLayout(scene.GetRegistry(), {1280, 720});
        for (const auto &entry : layout.Rects)
            Check(!entry.Enabled, "disabled canvas");
        Check(UI::CalculateLayout(scene.GetRegistry(), {0, 720}).Rects.empty(), "zero viewport");
        Check(UI::CalculateLayout(scene.GetRegistry(), {-1, 720}).Rects.empty(), "negative viewport");
        settings.ReferenceResolution.x = 0;
        layout = UI::CalculateLayout(scene.GetRegistry(), {1280, 720});
        Check(layout.Rects.empty() && !layout.Diagnostics.empty(), "invalid canvas quarantined");
        settings.ReferenceResolution.x = 1280;
        rect.SizeDelta.x = -2000;
        layout = UI::CalculateLayout(scene.GetRegistry(), {1280, 720});
        Check(layout.Rects.size() == 1 && !layout.Diagnostics.empty(), "negative child size quarantines descendants");
        rect.SizeDelta.x = 0;
        rect.AnchoredPosition.x = std::numeric_limits<float>::infinity();
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
        for (size_t i = 0; i < 5; ++i)
            Check(uint64_t(hierarchy.Nodes[i].ID) == expected[i], "stable depth-first order");
        Check(scene.SetSiblingOrder(b, -1), "set sibling order");
        hierarchy = UI::BuildHierarchy(scene.GetRegistry());
        Check(hierarchy.Nodes[2].ID == b.GetUUID(), "sibling reorder");
        Check(!scene.SetParent(a, a), "reject self parent");
        Check(!scene.SetParent(a, child), "reject cycle");
        Check(!scene.SetParent(root, other), "reject nested canvas");
        Scene foreign;
        auto foreignCanvas = foreign.CreateCanvas();
        Check(!scene.SetParent(a, foreignCanvas), "reject cross-scene parent");
        auto world = scene.CreateEntity("World");
        Check(!scene.CreateUIEntity(world), "reject non-UI parent");
        Check(scene.SetParent(a, other), "valid reparent across root canvases");
        hierarchy = UI::BuildHierarchy(scene.GetRegistry());
        Check(hierarchy.Nodes[2].Canvas == other.GetUUID(), "descendant changes canvas");
        // Simulate corrupt serialized fields that bypass the checked mutation API.
        ForceParent(scene, a, child.GetUUID());
        hierarchy = UI::BuildHierarchy(scene.GetRegistry());
        Check(hierarchy.Nodes.size() == 3 && hierarchy.Diagnostics.size() == 2, "cycle quarantined");
        ForceParent(scene, a, UUID(999));
        Check(UI::BuildHierarchy(scene.GetRegistry()).Diagnostics.size() == 2, "missing parent quarantines subtree");
        ForceParent(scene, a, root.GetUUID());
        a.AddComponent<CanvasComponent>();
        Check(UI::BuildHierarchy(scene.GetRegistry()).Diagnostics.size() == 2, "nested canvas quarantines subtree");
        a.RemoveComponent<CanvasComponent>();
        ForceParent(scene, a, other.GetUUID()); // restore the original index-consistent relationship
        auto brokenCanvas = scene.CreateEntity("BrokenCanvas");
        scene.GetRegistry().emplace<CanvasComponent>(brokenCanvas); // malformed input diagnostic
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
        Check(copy.GetComponent<RelationshipComponent>().GetParent() == canvasID, "external parent preserved");
        auto copiedChild = scene->FindEntityByName("Child_Copy");
        Check(copiedChild && copiedChild.GetComponent<RelationshipComponent>().GetParent() == copy.GetUUID(), "internal parent remapped");
        Check(!copy.GetComponent<RectTransformComponent>().IsEnabled(), "enabled copied");
        auto copiedScene = Scene::Copy(scene);
        Check(copiedScene->GetEntityByUUID(childID).GetComponent<RelationshipComponent>().GetParent() == panelID,
              "scene copy preserves UUID relationships");
        auto fullRootCopy = scene->DuplicateUISubtree(canvas);
        Check(fullRootCopy && uint64_t(fullRootCopy.GetComponent<RelationshipComponent>().GetParent()) == 0, "canvas copy stays root");
        auto hierarchy = UI::BuildHierarchy(scene->GetRegistry());
        Check(hierarchy.Diagnostics.empty(), "copied hierarchy valid");
        // The editor still uses TagComponent directly; it must get cascade semantics too.
        panel.GetComponent<TagComponent>().SetRemove(true);
        scene->FlushPendingEntityDestruction();
        Check(!scene->GetEntityByUUID(panelID) && !scene->GetEntityByUUID(childID), "cascade deletion");
        Check(scene->GetEntityByUUID(sibling.GetUUID()) && scene->GetEntityByUUID(copy.GetUUID()), "unrelated branches survive");
        Check(copiedScene->GetEntityByUUID(panelID) && copiedScene->GetEntityByUUID(childID), "scene copies isolated");
        Check(!scene->SetSiblingOrder(child, 1), "stale entity rejected");
        scene->DestroyEntity(canvas);
        scene->FlushPendingEntityDestruction();
        Check(UI::BuildHierarchy(scene->GetRegistry()).Diagnostics.empty(), "no orphans after deleting canvas");
        Check(scene->GetEntityByUUID(fullRootCopy.GetUUID()), "other canvas survives");
    }

    struct DestroyProbe : ScriptEntity
    {
        std::vector<int> *Events = nullptr;
        int ID = 0;
        void OnDestroy() override { Events->push_back(ID); }
    };

    void GeneralHierarchy()
    {
        static_assert(std::is_const_v<std::remove_reference_t<decltype(std::declval<Entity>().GetComponent<RelationshipComponent>())>>);
        auto scene = CreateRef<Scene>();
        auto a = scene->CreateEntity("A");
        auto b = scene->CreateEntity("B");
        auto c = scene->CreateEntity("C");
        auto leaf = scene->CreateEntity("Leaf");
        Check(b.SetParent(a) && c.SetParent(a) && leaf.SetParent(b), "ordinary entity parenting");
        Check(a.GetChildren().size() == 2 && b.GetChildren().size() == 1, "child index maintained");
        Check(b.GetParent() == a && leaf.GetParent() == b, "parent queries");
        Check(scene->GetRootEntities().size() == 1, "root index maintained");
        Check(c.SetSiblingIndex(0) && a.GetChildren()[0] == c && b.GetSiblingIndex() == 1, "index reorder");
        Check(c.SetSiblingIndex(999) && a.GetChildren().back() == c, "index clamps");
        scene->SetSiblingOrder(c, -7);
        Check(a.GetChildren().front() == c, "signed sibling order");
        auto before = a.GetChildren();
        std::string error;
        Check(!a.SetParent(leaf, &error) && !error.empty() && a.GetChildren() == before, "cycle rejected without mutations");
        Check(b.Detach() && !b.GetParent() && scene->GetRootEntities().size() == 2, "detach updates roots");
        Check(leaf.GetParent() == b && a.GetChildren().size() == 1, "detach retains subtree");
        Check(b.SetParent(a), "reattach");
        auto copied = scene->DuplicateSubtree(b);
        Check(copied.GetParent() == a && copied.GetChildren().size() == 1, "ordinary subtree copied");
        Check(copied.GetChildren()[0].GetUUID() != leaf.GetUUID(), "subtree UUID remapped");
        auto clone = Scene::Copy(scene);
        Check(clone->GetEntityByUUID(b.GetUUID()).GetChildren().size() == 1, "scene copy rebuilds child index");
        Check(clone->GetEntityByUUID(b.GetUUID()).Detach() && b.GetParent() == a, "copied indices isolated");

        std::vector<int> destroyed;
        const auto probe = [&](Entity entity, int id)
        {
            auto &nsc = entity.AddComponent<NativeScriptComponent>();
            nsc.Bind<DestroyProbe>();
            auto *instance = new DestroyProbe;
            instance->Events = &destroyed;
            instance->ID = id;
            nsc.Instance = instance;
        };
        probe(b, 1);
        probe(leaf, 2);
        b.Destroy();
        Check(!leaf.Detach() && !leaf.SetSiblingIndex(0), "pending subtree cannot escape deletion");
        scene->FlushPendingEntityDestruction();
        Check(!b && !leaf && destroyed == std::vector<int>({2, 1}), "children destroyed before parent");
        Check(a.GetChildren().size() == 2 && copied, "delete cleans parent index only");
        Check(!c.SetParent(b), "stale parent is not treated as detach");
        a.DestroyChildren();
        scene->FlushPendingEntityDestruction();
        Check(a && a.GetChildren().empty() && !c && !copied, "destroy children preserves parent");
        auto bare = scene->CreateEntityWithID(UUID(500));
        Check(bare.SetParent(a), "ID-only entity hierarchy");
        bare.Destroy();
        scene->FlushPendingEntityDestruction();
        Check(!bare && a.GetChildren().empty(), "deletion without TagComponent");

        b2WorldDef worldDef = b2DefaultWorldDef();
        auto world = b2CreateWorld(&worldDef);
        auto physical = scene->CreateEntity("Physical");
        auto &rb = physical.AddComponent<Rigidbody2DComponent>();
        b2BodyDef bodyDef = b2DefaultBodyDef();
        rb.RuntimeBodyId = b2CreateBody(world, &bodyDef);
        auto body = rb.RuntimeBodyId;
        auto physicalCopy = scene->DuplicateSubtree(physical);
        Check(!b2Body_IsValid(physicalCopy.GetComponent<Rigidbody2DComponent>().RuntimeBodyId), "body handles are not copied");
        physicalCopy.Destroy();
        scene->FlushPendingEntityDestruction();
        Check(b2Body_IsValid(body), "deleting copy preserves source body");
        physical.Destroy();
        scene->FlushPendingEntityDestruction();
        Check(!b2Body_IsValid(body), "deleting entity removes physics body");
        b2DestroyWorld(world);
    }

    void RelationshipPersistence(const std::filesystem::path &directory)
    {
        auto scene = CreateRef<Scene>();
        auto root = scene->CreateEntity("Root");
        auto child = scene->CreateEntity("Child");
        auto leaf = scene->CreateEntity("Leaf");
        child.SetParent(root);
        leaf.SetParent(child);
        scene->SetSiblingOrder(child, -4);
        auto path = directory / "relationships.scene";
        Serialization::SceneSerialize(scene).Serialize(path.string());
        auto loaded = CreateRef<Scene>();
        Check(Serialization::SceneSerialize(loaded).Deserialize(path.string()), "ordinary hierarchy reload");
        auto loadedChild = loaded->GetEntityByUUID(child.GetUUID());
        Check(loadedChild.GetParent().GetUUID() == root.GetUUID() && loadedChild.GetChildren().size() == 1, "reloaded indices");
        loadedChild.Destroy();
        loaded->FlushPendingEntityDestruction();
        Check(loaded->GetEntityByUUID(root.GetUUID()).GetChildren().empty(), "reloaded cascade delete");

        const auto legacyPath = directory / "legacy-ui.scene";
        std::ofstream(legacyPath) << "Scene: LegacyUI\nEntities:\n"
            "  - EntityID: 10\n    TagComponent: {Tag: Child}\n    RectTransformComponent: {Parent: 20, SiblingOrder: -9}\n"
            "  - EntityID: 20\n    TagComponent: {Tag: Canvas}\n    CanvasComponent: {}\n    RectTransformComponent: {Parent: 0}\n";
        auto legacy = CreateRef<Scene>();
        Check(Serialization::SceneSerialize(legacy).Deserialize(legacyPath.string()), "legacy UI migration");
        auto legacyChild = legacy->GetEntityByUUID(UUID(10));
        Check(legacyChild.GetParent().GetUUID() == UUID(20) && legacyChild.GetComponent<RelationshipComponent>().GetSiblingOrder() == -9, "legacy fields migrated");
        Check(UI::CalculateLayout(legacy->GetRegistry(), {1280, 720}).Rects.size() == 2, "legacy UI still lays out");
        auto migratedPath = directory / "migrated-ui.scene";
        Serialization::SceneSerialize(legacy).Serialize(migratedPath.string());
        auto migrated = YAML::LoadFile(migratedPath.string());
        auto saved = migrated["Entities"][0];
        Check(saved["RelationshipComponent"] && !saved["RectTransformComponent"]["Parent"] && !saved["RectTransformComponent"]["SiblingOrder"], "only Relationship stores hierarchy");
        // A new-format relationship must take precedence over leftover legacy fields.
        saved["RectTransformComponent"]["Parent"] = 999;
        std::ofstream(migratedPath) << migrated;
        auto precedence = CreateRef<Scene>();
        Check(Serialization::SceneSerialize(precedence).Deserialize(migratedPath.string()), "new relationship takes precedence");
        Check(precedence->GetEntityByUUID(UUID(10)).GetParent().GetUUID() == UUID(20), "precedence parent");
        for (auto parent : {10, 999})
        {
            saved["RelationshipComponent"]["Parent"] = parent;
            std::ofstream(migratedPath) << migrated;
            auto invalid = CreateRef<Scene>();
            Check(!Serialization::SceneSerialize(invalid).Deserialize(migratedPath.string()), "reject cyclic or missing parent load");
        }
        saved["EntityID"] = 20;
        std::ofstream(migratedPath) << migrated;
        auto duplicate = CreateRef<Scene>();
        Check(!Serialization::SceneSerialize(duplicate).Deserialize(migratedPath.string()), "reject duplicate UUID load");
    }

    void ExclusiveTransforms(const std::filesystem::path &directory)
    {
        auto scene = CreateRef<Scene>();
        auto world = scene->CreateEntity("World");
        auto canvas = scene->CreateCanvas();
        auto image = scene->CreateUIEntity(canvas);
        image.AddComponent<UIImageComponent>();
        Check(world.HasComponent<TransformComponent>() && !world.HasComponent<RectTransformComponent>(), "world has only Transform");
        Check(!canvas.HasComponent<TransformComponent>() && !image.HasComponent<TransformComponent>(), "UI has only RectTransform");
        const auto rejected = [](auto operation)
        {
            try { operation(); } catch (const std::logic_error &) { return true; }
            return false;
        };
        Check(rejected([&] { image.AddComponent<TransformComponent>(); }), "cannot add world transform to UI");
        Check(rejected([&] { world.AddComponent<RectTransformComponent>(); }), "cannot add RectTransform to world entity");
        Check(rejected([&] { image.AddOrReplaceComponent<TransformComponent>(); }), "replace cannot bypass exclusivity");
        Check(rejected([&] { world.AddOrReplaceComponent<RectTransformComponent>(); }), "reverse replace cannot bypass exclusivity");
        Check(rejected([&] { image.AddComponent<Rigidbody2DComponent>(); }), "UI rejects physics");
        Check(rejected([&] { world.AddComponent<UIImageComponent>(); }), "world rejects UI graphic");
        Check(rejected([&] { image.RemoveComponent<RectTransformComponent>(); }), "UI dependency guards removal");
        world.AddComponent<SpriteComponent>();
        Check(rejected([&] { world.RemoveComponent<TransformComponent>(); }), "world dependency guards removal");
        Check(!image.CanAddComponent<CameraComponent>() && image.CanAddComponent<UIImageComponent>(), "editor eligibility uses same rules");
        auto copy = scene->DuplicateSubtree(canvas);
        Check(copy && !copy.HasComponent<TransformComponent>() && !copy.GetChildren()[0].HasComponent<TransformComponent>(), "UI subtree copy preserves exclusivity");
        auto worldCopy = scene->DuplicateSubtree(world);
        Check(worldCopy.HasComponent<TransformComponent>() && !worldCopy.HasComponent<RectTransformComponent>(), "world copy retains Transform");
        auto cloned = Scene::Copy(scene);
        Check(!cloned->GetEntityByUUID(image.GetUUID()).HasComponent<TransformComponent>(), "scene copy preserves UI component set");
        const auto file = directory / "exclusive.scene";
        Serialization::SceneSerialize(scene).Serialize(file.string());
        auto loaded = CreateRef<Scene>();
        Check(Serialization::SceneSerialize(loaded).Deserialize(file.string()), "exclusive round trip");
        Check(!loaded->GetEntityByUUID(image.GetUUID()).HasComponent<TransformComponent>(), "reload does not inject Transform");
        for (const auto &node : YAML::LoadFile(file.string())["Entities"])
            Check(!(node["TransformComponent"] && node["RectTransformComponent"]), "only one serialized transform");
        const auto legacy = directory / "dual-transform.scene";
        const std::string prefix = "Scene: Legacy\nEntities:\n  - EntityID: 1\n    TagComponent: {Tag: UI}\n    CanvasComponent: {}\n    RectTransformComponent: {}\n";
        std::ofstream(legacy) << prefix << "    TransformComponent: {}\n";
        auto dualTransform = CreateRef<Scene>();
        Check(!Serialization::SceneSerialize(dualTransform).Deserialize(legacy.string()), "dual transforms rejected even with default values");
        for (const std::string extra : {
            "    TransformComponent: {Position: [1, 0, 0]}\n",
            "    TransformComponent: {Rotation: [0, 0, 30]}\n",
            "    TransformComponent: {Scale: [2, 2, 1]}\n",
            "    TransformComponent: {Enabled: false}\n",
            "    TransformComponent: {}\n    Rigidbody2DComponent: {}\n",
            "    Rigidbody2DComponent: {}\n"})
        {
            std::ofstream(legacy) << prefix << extra;
            auto conflict = CreateRef<Scene>();
            Check(!Serialization::SceneSerialize(conflict).Deserialize(legacy.string()), "conflicting or missing world transform rejected");
        }
        const auto *fields = Reflect<CanvasComponent>().fields;
        Check(std::count_if(fields->begin(), fields->end(), [](const auto &field) { return std::string_view(field.name) == "Enabled"; }) == 1, "Canvas Enabled registered once");
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
        scene->SetSiblingOrder(panel, -3);
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
        Check(c.SortOrder == 12 && Near(c.MatchWidthOrHeight, 0.25f) && loaded->GetEntityByUUID(UUID(200)).GetComponent<RelationshipComponent>().GetSiblingOrder() == -3 &&
                  Near(r.AnchoredPosition, {13, 27}),
              "component values persisted");
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
        GeneralHierarchy();
        RelationshipPersistence(directory);
        ExclusiveTransforms(directory);
        ENGINE_INFO("PASS: UI layout, scaling, hierarchy, ordering, copy, deletion and scene persistence");
        return 0;
    }
    catch (const std::exception &error)
    {
        ENGINE_ERROR("UI P1 test failed: {}", error.what());
        return 1;
    }
}
