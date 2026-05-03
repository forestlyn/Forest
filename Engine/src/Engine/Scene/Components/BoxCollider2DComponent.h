#pragma once
#include "BaseComponent.h"
#include <glm/glm.hpp>
namespace Engine
{
    struct BoxCollider2DComponent : public BaseComponent
    {

        /// @brief 密度，影响物体的质量，单位为 kg/m²
        float Density = 1.0f;
        /// @brief 摩擦力，影响物体之间的摩擦效果
        float Friction = 0.5f;
        /// @brief 恢复力，影响物体碰撞后的弹性效果
        float Restitution = 0.0f;
        /// @brief 恢复力阈值，碰撞速度低于该值时不应用恢复力，避免物体在静止或低速时抖动
        float RestitutionThreshold = 0.5f;

        /// @brief 碰撞框的偏移，单位为米
        glm::vec2 Offset = glm::vec2(0.0f);
        /// @brief 碰撞框的尺寸，单位为米
        glm::vec2 Size = glm::vec2(1.0f);

        BoxCollider2DComponent() = default;
        BoxCollider2DComponent(const BoxCollider2DComponent &other) = default;
    };

    REFLECT_TYPE_BEGIN(BoxCollider2DComponent)
    REFLECT_FIELD(Density);
    REFLECT_FIELD(Friction).UI(0.0f, 1.0f, 0.01f);
    REFLECT_FIELD(Restitution).UI(0.0f, 1.0f, 0.01f);
    REFLECT_FIELD(RestitutionThreshold).UI(0.0f, 1.0f, 0.01f);
    REFLECT_FIELD(Offset);
    REFLECT_FIELD(Size);
    REFLECT_TYPE_END(BoxCollider2DComponent)
} // namespace Engine
