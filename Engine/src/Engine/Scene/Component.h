#pragma once
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "Engine/Scene/SceneCamera.h"
namespace EngineEditor
{
    class SceneHierarchyPanel;
}
namespace Engine
{

    struct SpriteComponent
    {
        glm::vec4 Color{1.0f};
        SpriteComponent() = default;
        SpriteComponent(const SpriteComponent &other) = delete;
        SpriteComponent(const glm::vec4 &color) : Color(color) {}
    };

    struct TransformComponent
    {
    private:
        friend class EngineEditor::SceneHierarchyPanel;

        bool dirty = true;

        glm::mat4 TransformMatrix{1.0f};

        glm::mat4 CalculateTransform()
        {
            glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(Rotation)));

            return glm::translate(glm::mat4(1.0f), Position) *
                   rotation *
                   glm::scale(glm::mat4(1.0f), Scale);
        }

        glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 Scale = glm::vec3(1.0f, 1.0f, 1.0f);

    public:
        TransformComponent() = default;
        TransformComponent(const TransformComponent &other) = delete;
        TransformComponent(const glm::vec3 &position) : Position(position) {}

        glm::mat4 GetTransform()
        {
            if (dirty)
            {
                TransformMatrix = CalculateTransform();
                dirty = false;
            }
            return TransformMatrix;
        }

        void SetPosition(const glm::vec3 &position)
        {
            Position = position;
            dirty = true;
        }
        glm::vec3 GetPosition() const { return Position; }

        void SetRotation(const glm::vec3 &rotation)
        {
            Rotation = rotation;
            dirty = true;
        }
        glm::vec3 GetRotation() const { return Rotation; }

        void SetScale(const glm::vec3 &scale)
        {
            Scale = scale;
            dirty = true;
        }
        glm::vec3 GetScale() const { return Scale; }
    };

    struct TagComponent
    {
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent &other) = delete;
        TagComponent(const std::string &tag) : Tag(tag) {}
    };

    struct CameraComponent
    {
        Ref<SceneCamera> Camera;
        bool Primary = true; // Is this the primary camera?
        bool FixedAspectRatio = false;
        CameraComponent() = default;
        CameraComponent(const CameraComponent &other) = delete;
        CameraComponent(const Ref<SceneCamera> &camera) : Camera(camera) {}
    };
} // namespace Engine