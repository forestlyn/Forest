#include "Scene.h"
#include "Engine/Renderer/Renderer2D.h"
namespace Engine::Scene
{
    void Scene::OnUpdate(Core::Timestep deltaTime)
    {
        auto view = m_Registry.view<SpriteComponent, TransformComponent>();
        for (auto entity : view)
        {
            auto &spriteComponent = view.get<SpriteComponent>(entity);
            auto &transformComponent = view.get<TransformComponent>(entity);
            Renderer::Renderer2D::DrawQuad(transformComponent.GetTransform(), spriteComponent.Color);
        }
    }
}
