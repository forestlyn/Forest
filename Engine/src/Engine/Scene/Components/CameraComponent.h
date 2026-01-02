#pragma once
#include "Engine/Scene/SceneCamera.h"
namespace Engine
{
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