#pragma once
#include "BaseComponent.h"
#include <glm/glm.hpp>
namespace Engine
{
    struct BoxCollider2DComponent : public BaseComponent
    {
        float Density = 1.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f;
        float RestitutionThreshold = 0.5f;

        glm::vec2 Offset = glm::vec2(0.0f);
        glm::vec2 Size = glm::vec2(1.0f);

        BoxCollider2DComponent() = default;
        BoxCollider2DComponent(const BoxCollider2DComponent &other) = default;
    };
} // namespace Engine