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

    bool DeserializeTagComponent(const YAML::Node &componentNode, TagComponent &component);
    bool DeserializeTransformComponent(const YAML::Node &componentNode, TransformComponent &component);
    bool DeserializeCameraComponent(const YAML::Node &componentNode, CameraComponent &component);
    bool DeserializeSpriteComponent(const YAML::Node &componentNode, SpriteComponent &component);

} // namespace Engine::Serialization