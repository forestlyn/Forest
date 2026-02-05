#pragma once
#include "yaml-cpp/yaml.h"
#include "Engine/Scene/Component.h"
#include "SerializeUtils.h"
namespace Engine::Serialization
{
    void SerializeTagComponent(YAML::Emitter &out, TagComponent &entity);
    void SerializeTransformComponent(YAML::Emitter &out, TransformComponent &component);
    void SerializeCameraComponent(YAML::Emitter &out, CameraComponent &component);
    void SerializeSpriteComponent(YAML::Emitter &out, SpriteComponent &component);
    void SerializeCircleComponent(YAML::Emitter &out, CircleComponent &component);
    void SerializeRigidbody2DComponent(YAML::Emitter &out, Rigidbody2DComponent &component);
    void SerializeBoxCollider2DComponent(YAML::Emitter &out, BoxCollider2DComponent &component);
    void SerializeCircleCollider2DComponent(YAML::Emitter &out, CircleCollider2DComponent &component);
    void SerializeScriptComponent(YAML::Emitter &out, ScriptComponent &component);

    bool DeserializeTagComponent(const YAML::Node &componentNode, TagComponent &component);
    bool DeserializeTransformComponent(const YAML::Node &componentNode, TransformComponent &component);
    bool DeserializeCameraComponent(const YAML::Node &componentNode, CameraComponent &component);
    bool DeserializeSpriteComponent(const YAML::Node &componentNode, SpriteComponent &component);
    bool DeserializeCircleComponent(const YAML::Node &componentNode, CircleComponent &component);
    bool DeserializeRigidbody2DComponent(const YAML::Node &componentNode, Rigidbody2DComponent &component);
    bool DeserializeBoxCollider2DComponent(const YAML::Node &componentNode, BoxCollider2DComponent &component);
    bool DeserializeCircleCollider2DComponent(const YAML::Node &componentNode, CircleCollider2DComponent &component);
    bool DeserializeScriptComponent(const YAML::Node &componentNode, ScriptComponent &component);
} // namespace Engine::Serialization