#pragma once
#include "BaseComponent.h"
#include <glm/glm.hpp>
namespace Engine
{
    struct CircleCollider2DComponent : public BaseComponent
    {
        float Radius = 0.5f;
        glm::vec2 Offset = glm::vec2(0.0f, 0.0f);

        float Density = 1.0f;
        float Friction = 0.5f;
        float RollingResistance = 0.05f;
        float Restitution = 0.0f;
        float RestitutionThreshold = 0.5f;

        CircleCollider2DComponent() = default;
        CircleCollider2DComponent(const CircleCollider2DComponent &) = default;
    };

    REFLECT_TYPE_BEGIN(CircleCollider2DComponent)
    REFLECT_FIELD(Radius)
    REFLECT_FIELD(Offset)
    REFLECT_FIELD(Density)
    REFLECT_FIELD(Friction)
    REFLECT_FIELD(RollingResistance)
    REFLECT_FIELD(Restitution)
    REFLECT_FIELD(RestitutionThreshold)
    REFLECT_TYPE_END(CircleCollider2DComponent)
} // namespace Engine