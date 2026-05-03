#pragma once
#include "BaseComponent.h"
#include <glm/glm.hpp>
namespace Engine
{
    struct CircleCollider2DComponent : public BaseComponent
    {
        float Radius = 0.5f;
        glm::vec2 Offset = glm::vec2(0.0f, 0.0f);

        /// @brief 物理属性
        /// @brief 密度，影响物体的质量，单位为 kg/m²
        float Density = 1.0f;
        /// @brief 摩擦力，影响物体之间的摩擦效果
        float Friction = 0.5f;
        /// @brief 滚动阻力，影响物体在地面上滚动时的减速效果
        float RollingResistance = 0.05f;
        /// @brief 恢复力，影响物体碰撞后的弹性效果
        float Restitution = 0.0f;
        /// @brief 恢复力阈值，碰撞速度低于该值时不应用恢复力，避免物体在静止或低速时抖动
        float RestitutionThreshold = 0.5f;

        CircleCollider2DComponent() = default;
        CircleCollider2DComponent(const CircleCollider2DComponent &) = default;
    };

    REFLECT_TYPE_BEGIN(CircleCollider2DComponent)
    REFLECT_FIELD(Radius);
    REFLECT_FIELD(Offset);
    REFLECT_FIELD(Density);
    REFLECT_FIELD(Friction);
    REFLECT_FIELD(RollingResistance);
    REFLECT_FIELD(Restitution);
    REFLECT_FIELD(RestitutionThreshold);
    REFLECT_TYPE_END(CircleCollider2DComponent)
} // namespace Engine
