#pragma once
#include <box2d/box2d.h>
#include "BaseComponent.h"
#include <glm/glm.hpp>
class b2Body;
namespace Engine
{
    enum class Rigidbody2DBodyType
    {
        Static = b2_staticBody,
        Kinematic = b2_kinematicBody,
        Dynamic = b2_dynamicBody
    };
    struct Rigidbody2DComponent : public BaseComponent
    {
        Rigidbody2DBodyType Type = Rigidbody2DBodyType::Static;
        glm::vec2 Velocity = glm::vec2(0.0f, 9.8f);
        float AngularVelocity = 0.0f;
        bool FixedRotation = false;

        b2BodyId RuntimeBodyId;

        Rigidbody2DComponent() = default;
        Rigidbody2DComponent(const Rigidbody2DComponent &other) = default;
    };

    REFLECT_ENUM_BEGIN(Rigidbody2DBodyType)
    REFLECT_ENUM_VALUE(Static)
    REFLECT_ENUM_VALUE(Kinematic)
    REFLECT_ENUM_VALUE(Dynamic)
    REFLECT_ENUM_END(Rigidbody2DBodyType)

    REFLECT_TYPE_BEGIN(Rigidbody2DComponent)
    REFLECT_FIELD(Type)
    REFLECT_FIELD(Velocity)
    REFLECT_FIELD(AngularVelocity)
    REFLECT_FIELD(FixedRotation)
    REFLECT_TYPE_END(Rigidbody2DComponent)
} // namespace Engine