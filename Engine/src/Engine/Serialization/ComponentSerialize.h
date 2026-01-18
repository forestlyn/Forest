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
    void SerilizaRigidbody2DComponent(YAML::Emitter &out, Rigidbody2DComponent &component);
    void SerilizaBoxCollider2DComponent(YAML::Emitter &out, BoxCollider2DComponent &component);

    bool DeserializeTagComponent(const YAML::Node &componentNode, TagComponent &component);
    bool DeserializeTransformComponent(const YAML::Node &componentNode, TransformComponent &component);
    bool DeserializeCameraComponent(const YAML::Node &componentNode, CameraComponent &component);
    bool DeserializeSpriteComponent(const YAML::Node &componentNode, SpriteComponent &component);
    bool DeserializeRigidbody2DComponent(const YAML::Node &componentNode, Rigidbody2DComponent &component);
    bool DeserializeBoxCollider2DComponent(const YAML::Node &componentNode, BoxCollider2DComponent &component);

} // namespace Engine::Serialization