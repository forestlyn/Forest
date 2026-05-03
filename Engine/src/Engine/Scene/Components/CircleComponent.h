#pragma once
#include "BaseComponent.h"
#include <glm/glm.hpp>
namespace Engine
{
    struct CircleComponent : public BaseComponent
    {
        /// @brief 圆形的颜色
        glm::vec4 Color = glm::vec4(1.0f);
        /// @brief 圆形边缘的厚度，单位为像素
        float Thickness = 1.0f;
        /// @brief 圆形边缘的渐变模糊程度，单位为像素
        float Fade = 0.005f;

        CircleComponent() = default;
        CircleComponent(const CircleComponent &other) = default;
    };

    REFLECT_TYPE_BEGIN(CircleComponent)
    REFLECT_FIELD(Color).UIKIND(UIKind::UITYPE_Color);
    REFLECT_FIELD(Thickness);
    REFLECT_FIELD(Fade);
    REFLECT_TYPE_END(CircleComponent)
}
