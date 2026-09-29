#include "UIRenderer.h"
#include "Engine/Scene/Components/IDComponent.h"
#include "Engine/Scene/Components/UIImageComponent.h"
#include "Engine/Renderer/Renderer2D.h"
#include "Engine/Renderer/RenderCommand.h"
#include "Engine/Resource/ResourceManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>
#include <cmath>

namespace Engine::UI
{
    void RenderImages(entt::registry &registry, const Layout &layout, glm::vec2 viewportPixels)
    {
        if (layout.Rects.empty() || !std::isfinite(viewportPixels.x) || !std::isfinite(viewportPixels.y) ||
            viewportPixels.x <= 0 || viewportPixels.y <= 0) return;

        std::unordered_map<uint64_t, entt::entity> images;
        for (auto entity : registry.view<IDComponent, UIImageComponent>())
            images.emplace(uint64_t(registry.get<IDComponent>(entity).ID), entity);
        if (images.empty()) return;

        // Resolve resources before beginning the pass. CPU data is never referenced by a GPU callback.
        struct DrawItem { glm::mat4 Transform; glm::vec4 Color; Ref<Renderer::Texture2D> Texture; int EntityID; };
        std::vector<DrawItem> items;
        for (const auto &rect : layout.Rects)
        {
            auto found = images.find(uint64_t(rect.EntityID));
            if (!rect.Enabled || found == images.end() || rect.Size.x <= 0 || rect.Size.y <= 0) continue;
            auto &image = registry.get<UIImageComponent>(found->second);
            if (!image.IsEnabled() || image.Color.a <= 0) continue;
            if (image.TextureRef.IsValid() && !image.TextureRef.IsLoaded() && ResourceManager::Get())
                image.TextureRef.instance = ResourceManager::Get()->GetOrLoad<Renderer::Texture2D>(image.TextureRef.path);
            glm::vec2 size = rect.Size * rect.CanvasScale;
            glm::vec2 center = (rect.Min + rect.Size * 0.5f) * rect.CanvasScale;
            // Quad UV (0,0) is bottom-left, while UI Y increases downwards.
            auto transform = glm::translate(glm::mat4(1.0f), glm::vec3(center, 0.0f)) *
                             glm::scale(glm::mat4(1.0f), glm::vec3(size.x, -size.y, 1.0f));
            items.push_back({transform, image.Color, image.TextureRef.IsValid() ? image.TextureRef.instance : nullptr,
                             static_cast<int>(found->second)});
        }
        if (items.empty()) return;
        Renderer::RenderCommand::BeginOverlay();
        Renderer::Renderer2D::BeginScene(glm::ortho(0.0f, viewportPixels.x, viewportPixels.y, 0.0f, -1.0f, 1.0f));
        for (const auto &item : items)
        {
            if (item.Texture)
                Renderer::Renderer2D::DrawQuad(item.Transform, item.Texture, 1.0f, item.Color, item.EntityID);
            else
                Renderer::Renderer2D::DrawQuad(item.Transform, item.Color, item.EntityID);
        }
        Renderer::Renderer2D::EndScene();
        Renderer::RenderCommand::EndOverlay();
    }
}
