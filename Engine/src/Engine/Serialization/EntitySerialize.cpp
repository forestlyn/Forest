#include "EntitySerialize.h"

namespace Engine::Serialization
{
    void SerilizaEntity(YAML::Emitter &out, Entity entity)
    {
        out << YAML::BeginMap; // Entity
        out << YAML::Key << "EntityID" << YAML::Value << (uint32_t)(entt::entity)entity;

        if (entity.HasComponent<TagComponent>())
        {
            auto &tagComp = entity.GetComponent<TagComponent>();
            SerilizaTagComponent(out, tagComp);
        }
        if (entity.HasComponent<TransformComponent>())
        {
            auto &transformComp = entity.GetComponent<TransformComponent>();
            SerilizaTransformComponent(out, transformComp);
        }
        if (entity.HasComponent<CameraComponent>())
        {
            auto &cameraComp = entity.GetComponent<CameraComponent>();
            SerilizaCameraComponent(out, cameraComp);
        }
        if (entity.HasComponent<SpriteComponent>())
        {
            auto &spriteComp = entity.GetComponent<SpriteComponent>();
            SerilizaSpriteComponent(out, spriteComp);
        }
        out << YAML::EndMap; // Entity
    }

    bool DeserializeEntity(const YAML::Node &entityNode, Entity &entity)
    {
        // 反序列化组件
        if (entityNode["TagComponent"])
        {
            auto tagNode = entityNode["TagComponent"];
            auto &tagComp = entity.AddComponent<TagComponent>();
            if (!DeserializeTagComponent(tagNode, tagComp))
            {
                return false;
            }
        }
        if (entityNode["TransformComponent"])
        {
            auto transformNode = entityNode["TransformComponent"];
            auto &transformComp = entity.AddComponent<TransformComponent>();
            if (!DeserializeTransformComponent(transformNode, transformComp))
            {
                return false;
            }
        }
        if (entityNode["CameraComponent"])
        {
            auto cameraNode = entityNode["CameraComponent"];
            auto &cameraComp = entity.AddComponent<CameraComponent>();
            if (!DeserializeCameraComponent(cameraNode, cameraComp))
            {
                return false;
            }
        }
        if (entityNode["SpriteComponent"])
        {
            auto spriteNode = entityNode["SpriteComponent"];
            auto &spriteComp = entity.AddComponent<SpriteComponent>();
            if (!DeserializeSpriteComponent(spriteNode, spriteComp))
            {
                return false;
            }
        }
        return true;
    }
} // namespace Engine::Serialization