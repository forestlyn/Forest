#pragma once
#include <glm/glm.hpp>
#include "BaseComponent.h"
namespace Engine
{
    struct SpriteComponent : public BaseComponent
    {
        glm::vec4 Color{1.0f};
        SpriteComponent() = default;
        SpriteComponent(const SpriteComponent &other) = delete;
        SpriteComponent(const glm::vec4 &color) : Color(color) {}
    };
}