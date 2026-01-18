#pragma once
#include <box2d/box2d.h>
#include "BaseComponent.h"
#include <glm/glm.hpp>
class b2Body;
namespace Engine
{
    struct Rigidbody2DComponent : public BaseComponent
    {
        enum class BodyType
        {
            Static = b2_staticBody,
            Kinematic = b2_kinematicBody,
            Dynamic = b2_dynamicBody
        };

        BodyType Type = BodyType::Static;
        glm::vec2 Velocity = glm::vec2(0.0f, 9.8f);
        float AngularVelocity = 0.0f;
        bool FixedRotation = false;
        b2BodyId RuntimeBodyId;

        Rigidbody2DComponent() = default;
        Rigidbody2DComponent(const Rigidbody2DComponent &other) = delete;
    };
} // namespace Engine