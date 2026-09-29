#pragma once
#include "UILayout.h"

namespace Engine::UI
{
    // Called on the logic thread. All GPU work is submitted through Renderer2D/RenderCommand.
    void RenderImages(entt::registry &registry, const Layout &layout, glm::vec2 viewportPixels);
}
