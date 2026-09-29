#include "UILayout.h"
#include "Engine/Scene/Components/CanvasComponent.h"
#include "Engine/Scene/Components/RectTransformComponent.h"
#include "Engine/Scene/Components/IDComponent.h"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <unordered_set>

namespace Engine::UI
{
    namespace
    {
        bool Finite(glm::vec2 value) { return std::isfinite(value.x) && std::isfinite(value.y); }
        bool Unit(glm::vec2 value)
        {
            return Finite(value) && value.x >= 0 && value.x <= 1 && value.y >= 0 && value.y <= 1;
        }
    }

    Hierarchy BuildHierarchy(const entt::registry &registry)
    {
        Hierarchy result;
        std::unordered_map<uint64_t, entt::entity> entities;
        std::unordered_map<uint64_t, std::vector<entt::entity>> children;
        std::unordered_set<uint64_t> duplicateIDs;
        std::vector<entt::entity> roots;
        const auto id = [&](entt::entity e) { return uint64_t(registry.get<IDComponent>(e).ID); };
        for (auto e : registry.view<IDComponent>())
        {
            if (!entities.emplace(id(e), e).second) duplicateIDs.insert(id(e));
        }
        for (auto e : registry.view<CanvasComponent>())
            if (!registry.all_of<RectTransformComponent>(e))
                result.Diagnostics.push_back({registry.all_of<IDComponent>(e) ? UUID(id(e)) : UUID(0),
                                              "Canvas requires RectTransformComponent"});

        for (auto e : registry.view<RectTransformComponent>())
        {
            if (!registry.all_of<IDComponent>(e))
            {
                result.Diagnostics.push_back({UUID(0), "UI node requires IDComponent"});
                continue;
            }
            const auto &rect = registry.get<RectTransformComponent>(e);
            if (registry.all_of<CanvasComponent>(e) && uint64_t(rect.Parent.uuid) == 0)
                roots.push_back(e);
            else
                children[uint64_t(rect.Parent.uuid)].push_back(e);
        }
        const auto siblingLess = [&](entt::entity a, entt::entity b)
        {
            int x = registry.get<RectTransformComponent>(a).SiblingOrder;
            int y = registry.get<RectTransformComponent>(b).SiblingOrder;
            return x != y ? x < y : id(a) < id(b);
        };
        std::sort(roots.begin(), roots.end(), [&](auto a, auto b)
        {
            int x = registry.get<CanvasComponent>(a).SortOrder;
            int y = registry.get<CanvasComponent>(b).SortOrder;
            return x != y ? x < y : id(a) < id(b);
        });
        for (auto &[parent, list] : children) std::sort(list.begin(), list.end(), siblingLess);

        struct Pending { entt::entity Entity; UUID Canvas; };
        std::vector<Pending> stack;
        for (auto it = roots.rbegin(); it != roots.rend(); ++it) stack.push_back({*it, UUID(id(*it))});
        std::unordered_set<uint64_t> visited;
        while (!stack.empty())
        {
            auto [e, canvas] = stack.back();
            stack.pop_back();
            auto uuid = id(e);
            const auto &rect = registry.get<RectTransformComponent>(e);
            if (uuid == 0 || duplicateIDs.contains(uuid) || visited.contains(uuid)) continue;
            // A nested Canvas and its subtree must never leak into the parent's draw list.
            if (registry.all_of<CanvasComponent>(e) && uint64_t(rect.Parent.uuid) != 0) continue;
            visited.insert(uuid);
            result.Nodes.push_back({e, UUID(uuid), rect.Parent.uuid, canvas});
            auto found = children.find(uuid);
            if (found != children.end())
                for (auto it = found->second.rbegin(); it != found->second.rend(); ++it)
                    stack.push_back({*it, canvas});
        }
        for (auto e : registry.view<IDComponent, RectTransformComponent>())
        {
            if (visited.contains(id(e))) continue;
            const auto &rect = registry.get<RectTransformComponent>(e);
            std::string reason = "UI node has no valid root Canvas (cycle, orphan or invalid ancestor)";
            if (!id(e) || duplicateIDs.contains(id(e))) reason = "UI entity UUID is zero or duplicated";
            else if (registry.all_of<CanvasComponent>(e) && uint64_t(rect.Parent.uuid)) reason = "Nested Canvas is not supported";
            else if (uint64_t(rect.Parent.uuid) && !entities.contains(uint64_t(rect.Parent.uuid))) reason = "UI parent does not exist";
            result.Diagnostics.push_back({UUID(id(e)), reason});
        }
        std::sort(result.Diagnostics.begin(), result.Diagnostics.end(), [](const auto &a, const auto &b)
        { return uint64_t(a.EntityID) < uint64_t(b.EntityID); });
        return result;
    }

    Layout CalculateLayout(const entt::registry &registry, glm::vec2 viewportPixels)
    {
        auto hierarchy = BuildHierarchy(registry);
        Layout result;
        result.Diagnostics = std::move(hierarchy.Diagnostics);
        // Minimized or unavailable viewports produce no rectangles and no division by zero.
        if (!Finite(viewportPixels) || viewportPixels.x <= 0 || viewportPixels.y <= 0) return result;
        std::unordered_map<uint64_t, size_t> indices;
        for (const auto &node : hierarchy.Nodes)
        {
            const auto &rect = registry.get<RectTransformComponent>(node.Entity);
            LayoutRect computed;
            computed.EntityID = node.ID;
            computed.CanvasID = node.Canvas;
            computed.Enabled = rect.IsEnabled();
            if (uint64_t(node.Parent) == 0)
            {
                const auto &canvas = registry.get<CanvasComponent>(node.Entity);
                if (!Finite(canvas.ReferenceResolution) || canvas.ReferenceResolution.x <= 0 || canvas.ReferenceResolution.y <= 0 ||
                    !std::isfinite(canvas.MatchWidthOrHeight) || canvas.MatchWidthOrHeight < 0 || canvas.MatchWidthOrHeight > 1 ||
                    (canvas.ScaleMode != CanvasScaleMode::ConstantPixelSize && canvas.ScaleMode != CanvasScaleMode::ScaleWithScreenSize))
                {
                    result.Diagnostics.push_back({node.ID, "Invalid Canvas resolution, scale mode or match weight"});
                    continue;
                }
                if (canvas.ScaleMode == CanvasScaleMode::ScaleWithScreenSize)
                {
                    const glm::vec2 ratio = viewportPixels / canvas.ReferenceResolution;
                    computed.CanvasScale = std::exp2((1 - canvas.MatchWidthOrHeight) * std::log2(ratio.x) +
                                                     canvas.MatchWidthOrHeight * std::log2(ratio.y));
                }
                computed.Size = viewportPixels / computed.CanvasScale;
                computed.Enabled &= canvas.IsEnabled();
            }
            else
            {
                auto parent = indices.find(uint64_t(node.Parent));
                if (parent == indices.end()) continue; // Invalid ancestor quarantines the entire subtree.
                if (!Unit(rect.AnchorMin) || !Unit(rect.AnchorMax) || !Unit(rect.Pivot) ||
                    rect.AnchorMin.x > rect.AnchorMax.x || rect.AnchorMin.y > rect.AnchorMax.y ||
                    !Finite(rect.AnchoredPosition) || !Finite(rect.SizeDelta))
                {
                    result.Diagnostics.push_back({node.ID, "Invalid RectTransform anchors, pivot or offsets"});
                    continue;
                }
                const auto &p = result.Rects[parent->second];
                glm::vec2 start = p.Min + p.Size * rect.AnchorMin;
                glm::vec2 span = p.Size * (rect.AnchorMax - rect.AnchorMin);
                computed.Size = span + rect.SizeDelta;
                computed.Min = start + span * rect.Pivot + rect.AnchoredPosition - computed.Size * rect.Pivot;
                computed.CanvasScale = p.CanvasScale;
                computed.Enabled &= p.Enabled;
            }
            if (!Finite(computed.Min) || !Finite(computed.Size) || computed.Size.x < 0 || computed.Size.y < 0 ||
                !std::isfinite(computed.CanvasScale) || computed.CanvasScale <= 0)
            {
                result.Diagnostics.push_back({node.ID, "Layout produced a negative size or non-finite rectangle"});
                continue;
            }
            indices.emplace(uint64_t(node.ID), result.Rects.size());
            result.Rects.push_back(computed);
        }
        return result;
    }
}
