#pragma once
#include <glm/glm.hpp>
#include "BaseComponent.h"
#include "Engine/Renderer/Shader/Texture.h"
namespace Engine
{
    struct SpriteComponent : public BaseComponent
    {
        glm::vec4 Color{1.0f};
        float TilingFactor = 1.0f;
        Engine::Ref<Engine::Renderer::Texture2D> Texture = nullptr;
        SpriteComponent() = default;
        SpriteComponent(const SpriteComponent &other) = default;
        SpriteComponent(const glm::vec4 &color) : Color(color) {}
    };
}