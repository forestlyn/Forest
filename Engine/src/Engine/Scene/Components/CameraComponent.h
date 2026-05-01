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

    inline bool IsOrthographicCamera(const CameraComponent &camera)
    {
        return camera.ProjectionType == SceneCameraProjectionType::Orthographic;
    }

    inline bool IsPerspectiveCamera(const CameraComponent &camera)
    {
        return camera.ProjectionType == SceneCameraProjectionType::Perspective;
    }

    REFLECT_ENUM_BEGIN(SceneCameraProjectionType)
    REFLECT_ENUM_VALUE(Perspective)
    REFLECT_ENUM_VALUE(Orthographic)
    REFLECT_ENUM_END(SceneCameraProjectionType)

    REFLECT_TYPE_BEGIN(CameraComponent)
    REFLECT_FIELD(ProjectionType).UI().TOOLTIP("Camera projection mode.");
    REFLECT_FIELD(Primary).UI().TOOLTIP("Primary scene camera.");
    REFLECT_FIELD(FixedAspectRatio).UI().TOOLTIP("Keep the camera aspect ratio fixed.");
    REFLECT_FIELD(OrthographicSize).VISIBLEIF<&IsOrthographicCamera>().UI().TOOLTIP("Orthographic camera size.");
    REFLECT_FIELD(OrthographicNear).VISIBLEIF<&IsOrthographicCamera>().UI().TOOLTIP("Orthographic near clip plane.");
    REFLECT_FIELD(OrthographicFar).VISIBLEIF<&IsOrthographicCamera>().UI().TOOLTIP("Orthographic far clip plane.");
    REFLECT_FIELD(PerspectiveFOV).VISIBLEIF<&IsPerspectiveCamera>().UI().TOOLTIP("Perspective field of view.");
    REFLECT_FIELD(PerspectiveNear).VISIBLEIF<&IsPerspectiveCamera>().UI().TOOLTIP("Perspective near clip plane.");
    REFLECT_FIELD(PerspectiveFar).VISIBLEIF<&IsPerspectiveCamera>().UI().TOOLTIP("Perspective far clip plane.");
    REFLECT_FIELD(AspectRatio).UI(0, 100, 0.01f).TOOLTIP("Camera projection aspect ratio.");
    REFLECT_TYPE_END(CameraComponent)
} // namespace Engine
