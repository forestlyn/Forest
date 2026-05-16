#pragma once
#include <string>
namespace Engine::Renderer
{
    class Texture2D;
}
namespace Engine
{
    class Scene;
    class Entity;
    // 组件类
    class SpriteComponent;
    class CameraComponent;
    class TransformComponent;
}
namespace EngineEditor
{
    // 资源类
    constexpr const char *PAYLOAD_RES_TEXTURE = "RES_TEXTURE";

    // 场景类
    constexpr const char *PAYLOAD_SCENE = "SCENE";
    constexpr const char *PAYLOAD_SCENE_ENTITY = "SCENE_ENTITY";

    template <typename T>
    struct ResourcePayloadTrait
    {
        static constexpr const char *value = "";
    };

    // 为你的每种资源特化
    template <>
    struct ResourcePayloadTrait<Engine::Renderer::Texture2D>
    {
        static constexpr const char *value = PAYLOAD_RES_TEXTURE;
    };
    template <>
    struct ResourcePayloadTrait<Engine::Scene>
    {
        static constexpr const char *value = PAYLOAD_SCENE;
    };
    template <>
    struct ResourcePayloadTrait<Engine::Entity>
    {
        static constexpr const char *value = PAYLOAD_SCENE_ENTITY;
    };
}