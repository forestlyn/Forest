#pragma once
#include <glm/glm.hpp>
namespace Engine
{
    struct SpriteComponent
    {
        glm::vec4 Color{1.0f};
        SpriteComponent() = default;
        SpriteComponent(const SpriteComponent &other) = delete;
        SpriteComponent(const glm::vec4 &color) : Color(color) {}
    };
}