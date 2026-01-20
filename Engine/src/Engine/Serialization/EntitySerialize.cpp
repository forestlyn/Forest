#include "EntitySerialize.h"

namespace Engine::Serialization
{
    void SerilizaEntity(YAML::Emitter &out, Entity entity)
    {
        out << YAML::BeginMap; // Entity
        out << YAML::Key << "EntityID" << YAML::Value << entity.GetUUID();

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
        if (entity.HasComponent<CircleComponent>())
        {
            auto &circleComp = entity.GetComponent<CircleComponent>();
            SerilizaCircleComponent(out, circleComp);
        }
        if (entity.HasComponent<Rigidbody2DComponent>())
        {
            auto &rigidbody2DComp = entity.GetComponent<Rigidbody2DComponent>();
            SerilizaRigidbody2DComponent(out, rigidbody2DComp);
        }
        if (entity.HasComponent<BoxCollider2DComponent>())
        {
            auto &boxCollider2DComp = entity.GetComponent<BoxCollider2DComponent>();
            SerilizaBoxCollider2DComponent(out, boxCollider2DComp);
        }
        if (entity.HasComponent<CircleCollider2DComponent>())
        {
            auto &circleCollider2DComp = entity.GetComponent<CircleCollider2DComponent>();
            SerilizaCircleCollider2DComponent(out, circleCollider2DComp);
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
        if (entityNode["CircleComponent"])
        {
            auto circleNode = entityNode["CircleComponent"];
            auto &circleComp = entity.AddComponent<CircleComponent>();
            if (!DeserializeCircleComponent(circleNode, circleComp))
            {
                return false;
            }
        }
        if (entityNode["Rigidbody2DComponent"])
        {
            auto rigidbody2DNode = entityNode["Rigidbody2DComponent"];
            auto &rigidbody2DComp = entity.AddComponent<Rigidbody2DComponent>();
            if (!DeserializeRigidbody2DComponent(rigidbody2DNode, rigidbody2DComp))
            {
                return false;
            }
        }
        if (entityNode["BoxCollider2DComponent"])
        {
            auto boxCollider2DNode = entityNode["BoxCollider2DComponent"];
            auto &boxCollider2DComp = entity.AddComponent<BoxCollider2DComponent>();
            if (!DeserializeBoxCollider2DComponent(boxCollider2DNode, boxCollider2DComp))
            {
                return false;
            }
        }
        if (entityNode["CircleCollider2DComponent"])
        {
            auto circleCollider2DNode = entityNode["CircleCollider2DComponent"];
            auto &circleCollider2DComp = entity.AddComponent<CircleCollider2DComponent>();
            if (!DeserializeCircleCollider2DComponent(circleCollider2DNode, circleCollider2DComp))
            {
                return false;
            }
        }
        return true;
    }
} // namespace Engine::Serialization