#pragma once
#include "Engine/Core/UUID.h"
#include <entt.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace Engine::UI
{
    struct Diagnostic
    {
        UUID EntityID{0};
        std::string Message;
    };

    struct HierarchyNode
    {
        entt::entity Entity = entt::null;
        UUID ID{0};
        UUID Parent{0};
        UUID Canvas{0};
    };

    struct Hierarchy
    {
        // Stable parent-first depth-first order. Invalid subtrees are excluded.
        std::vector<HierarchyNode> Nodes;
        std::vector<Diagnostic> Diagnostics;
    };

    struct LayoutRect
    {
        UUID EntityID{0};
        UUID CanvasID{0};
        glm::vec2 Min{0.0f};
        glm::vec2 Size{0.0f};
        float CanvasScale = 1.0f;
        bool Enabled = true;
    };

    struct Layout
    {
        // Canvas logical units, origin at top left. Consumers must skip !Enabled.
        std::vector<LayoutRect> Rects;
        std::vector<Diagnostic> Diagnostics;
    };

    Hierarchy BuildHierarchy(const entt::registry &registry);
    Layout CalculateLayout(const entt::registry &registry, glm::vec2 viewportPixels);
}
