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
        CameraComponent() : Camera(Engine::CreateRef<SceneCamera>()) {}
        CameraComponent(const CameraComponent &other) = default;
        CameraComponent(const Ref<SceneCamera> &camera) : Camera(camera) {}
    };
} // namespace Engine