#pragma once
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "BaseComponent.h"
namespace EngineEditor
{
    class SceneHierarchyPanel;
}

namespace Engine
{
    struct TransformComponent : public BaseComponent
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
        TransformComponent(const TransformComponent &other) = default;
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

}