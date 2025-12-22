#pragma once
#include <glm/glm.hpp>
#include "Engine/Core/Application.h"
namespace Engine::Renderer
{
    class Camera
    {
    public:
        virtual ~Camera() = default;

        glm::vec2 ScreenToWorld(glm::vec2 screenPos) const
        {
            auto width = Engine::Core::Application::Get().GetWindowWidth();
            auto height = Engine::Core::Application::Get().GetWindowHeight();
            float ndc_x = (screenPos.x / (float)width) * 2.0f - 1.0f;
            float ndc_y = -((screenPos.y / (float)height) * 2.0f - 1.0f);
            glm::vec4 screenPosVec(ndc_x, ndc_y, 0.0f, 1.0f);
            glm::vec4 worldPos = m_InverseViewProjectionMatrix * screenPosVec;
            worldPos /= worldPos.w;
            return glm::vec2(worldPos.x, worldPos.y);
        }

        virtual void SetPosition(const glm::vec3 &position) = 0;
        virtual void SetRotationDegrees(const glm::vec3 &rotation) = 0;
        virtual glm::vec3 GetPosition() const { return m_Position; }
        virtual glm::vec3 GetRotationDegrees() const { return m_RotationDegrees; }

        virtual const glm::mat4 &GetProjectionMatrix() const { return m_ProjectionMatrix; }
        virtual const glm::mat4 &GetViewMatrix() const { return m_ViewMatrix; }
        virtual const glm::mat4 &GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

    protected:
        glm::vec3 m_Position = glm::vec3(0.0f);
        glm::vec3 m_RotationDegrees = glm::vec3(0.0f);
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;

        glm::mat4 m_ViewProjectionMatrix;
        glm::mat4 m_InverseViewProjectionMatrix;
    };
}