#pragma once
#include "BaseComponent.h"
#include <glm/glm.hpp>
namespace Engine
{
    struct CircleComponent : public BaseComponent
    {
        glm::vec4 Color = glm::vec4(1.0f);
        float Thickness = 1.0f;
        float Fade = 0.005f;

        CircleComponent() = default;
        CircleComponent(const CircleComponent &other) = default;
    };

    REFLECT_TYPE_BEGIN(CircleComponent)
    REFLECT_FIELD(Color)
    REFLECT_FIELD(Thickness)
    REFLECT_FIELD(Fade)
    REFLECT_TYPE_END(CircleComponent)
}