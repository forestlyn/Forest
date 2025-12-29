#pragma once
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "Engine/Renderer/Camera/Camera.h"
namespace Engine
{
    struct SpriteComponent
    {
        glm::vec4 Color{1.0f};
        SpriteComponent() = default;
        SpriteComponent(const SpriteComponent &other) = default;
        SpriteComponent(const glm::vec4 &color) : Color(color) {}
    };

    struct TransformComponent
    {
        glm::vec3 Position{0.0f, 0.0f, 0.0f};
        glm::vec3 Rotation{0.0f, 0.0f, 0.0f};
        glm::vec3 Scale{1.0f, 1.0f, 1.0f};

        TransformComponent() = default;
        TransformComponent(const TransformComponent &other) = default;
        TransformComponent(const glm::vec3 &position) : Position(position) {}

    private:
        bool dirty = true;
        glm::mat4 TransformMatrix{1.0f};

        glm::mat4 CalculateTransform()
        {
            glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(Rotation)));

            return glm::translate(glm::mat4(1.0f), Position) *
                   rotation *
                   glm::scale(glm::mat4(1.0f), Scale);
        }

    public:
        glm::mat4 GetTransform()
        {
            if (dirty)
            {
                TransformMatrix = CalculateTransform();
                dirty = false;
            }
            return TransformMatrix;
        }
    };

    struct TagComponent
    {
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent &other) = default;
        TagComponent(const std::string &tag) : Tag(tag) {}
    };

    struct CameraComponent
    {
        Ref<Renderer::Camera> Camera;
        bool Primary = true; // Is this the primary camera?

        CameraComponent() = default;
        CameraComponent(const CameraComponent &other) = default;
        CameraComponent(const Ref<Renderer::Camera> &camera) : Camera(camera) {}
    };
} // namespace Engine