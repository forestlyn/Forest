#pragma once
#include "BaseComponent.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace Engine
{
    enum class SceneCameraProjectionType
    {
        Perspective = 0,
        Orthographic = 1
    };

    struct CameraComponent : public BaseComponent
    {
        bool Primary = true; // Is this the primary camera?
        bool FixedAspectRatio = false;

        SceneCameraProjectionType ProjectionType = SceneCameraProjectionType::Orthographic;

        // Orthographic
        float OrthographicSize = 10.0f;
        float OrthographicNear = -1.0f;
        float OrthographicFar = 1.0f;

        // Perspective
        float PerspectiveFOV = 45.0f;
        float PerspectiveNear = 0.1f;
        float PerspectiveFar = 1000.0f;

        float AspectRatio = 16.0f / 9.0f;

        // 投影矩阵
        glm::mat4 ProjectionMatrix{1.0f};

        CameraComponent() { RecalculateProjection(); }
        CameraComponent(const CameraComponent &other) = default;

        void RecalculateProjection()
        {
            if (ProjectionType == SceneCameraProjectionType::Perspective)
            {
                ProjectionMatrix = glm::perspective(PerspectiveFOV, AspectRatio, PerspectiveNear, PerspectiveFar);
            }
            else
            {
                float orthoLeft = -OrthographicSize * AspectRatio * 0.5f;
                float orthoRight = OrthographicSize * AspectRatio * 0.5f;
                float orthoBottom = -OrthographicSize * 0.5f;
                float orthoTop = OrthographicSize * 0.5f;

                ProjectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, OrthographicNear, OrthographicFar);
            }
        }
    };

    REFLECT_ENUM_BEGIN(SceneCameraProjectionType)
    REFLECT_ENUM_VALUE(Perspective)
    REFLECT_ENUM_VALUE(Orthographic)
    REFLECT_ENUM_END(SceneCameraProjectionType)

    REFLECT_TYPE_BEGIN(CameraComponent)
    REFLECT_FIELD(ProjectionType)
    REFLECT_FIELD(Primary)
    REFLECT_FIELD(FixedAspectRatio)
    REFLECT_FIELD(OrthographicSize)
    REFLECT_FIELD(OrthographicNear)
    REFLECT_FIELD(OrthographicFar)
    REFLECT_FIELD(PerspectiveFOV)
    REFLECT_FIELD(PerspectiveNear)
    REFLECT_FIELD(PerspectiveFar)
    REFLECT_FIELD(AspectRatio)
    REFLECT_TYPE_END(CameraComponent)
} // namespace Engine
