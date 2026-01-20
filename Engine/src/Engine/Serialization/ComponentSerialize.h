#pragma once
#include "yaml-cpp/yaml.h"
#include "Engine/Scene/Component.h"
#include "SerializeUtils.h"
namespace Engine::Serialization
{
    void SerilizaTagComponent(YAML::Emitter &out, TagComponent &entity);
    void SerilizaTransformComponent(YAML::Emitter &out, TransformComponent &component);
    void SerilizaCameraComponent(YAML::Emitter &out, CameraComponent &component);
    void SerilizaSpriteComponent(YAML::Emitter &out, SpriteComponent &component);
    void SerilizaCircleComponent(YAML::Emitter &out, CircleComponent &component);
    void SerilizaRigidbody2DComponent(YAML::Emitter &out, Rigidbody2DComponent &component);
    void SerilizaBoxCollider2DComponent(YAML::Emitter &out, BoxCollider2DComponent &component);
    void SerilizaCircleCollider2DComponent(YAML::Emitter &out, CircleCollider2DComponent &component);

    bool DeserializeTagComponent(const YAML::Node &componentNode, TagComponent &component);
    bool DeserializeTransformComponent(const YAML::Node &componentNode, TransformComponent &component);
    bool DeserializeCameraComponent(const YAML::Node &componentNode, CameraComponent &component);
    bool DeserializeSpriteComponent(const YAML::Node &componentNode, SpriteComponent &component);
    bool DeserializeCircleComponent(const YAML::Node &componentNode, CircleComponent &component);
    bool DeserializeRigidbody2DComponent(const YAML::Node &componentNode, Rigidbody2DComponent &component);
    bool DeserializeBoxCollider2DComponent(const YAML::Node &componentNode, BoxCollider2DComponent &component);
    bool DeserializeCircleCollider2DComponent(const YAML::Node &componentNode, CircleCollider2DComponent &component);
} // namespace Engine::Serialization