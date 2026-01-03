#pragma once
#include "Engine/Scene/SceneCamera.h"
#include "BaseComponent.h"
namespace Engine
{
    struct CameraComponent : public BaseComponent
    {
        Ref<SceneCamera> Camera;
        bool Primary = true; // Is this the primary camera?
        bool FixedAspectRatio = false;
        CameraComponent() = default;
        CameraComponent(const CameraComponent &other) = delete;
        CameraComponent(const Ref<SceneCamera> &camera) : Camera(camera) {}
    };
} // namespace Engine