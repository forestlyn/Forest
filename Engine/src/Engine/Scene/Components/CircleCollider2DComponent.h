#pragma once
#include "BaseComponent.h"

namespace Engine
{
    struct CircleCollider2DComponent : public BaseComponent
    {
        float Radius = 0.5f;
        float Density = 1.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f;
        float RestitutionThreshold = 0.5f;

        CircleCollider2DComponent() = default;
        CircleCollider2DComponent(const CircleCollider2DComponent &) = default;
    };
} // namespace Engine