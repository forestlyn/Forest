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

    REFLECT_TYPE_BEGIN(CameraComponent)
    REFLECT_FIELD(Primary)
    REFLECT_FIELD(FixedAspectRatio)
    REFLECT_TYPE_END(CameraComponent)
} // namespace Engine