#pragma once
#include "Components/IDComponent.h"
#include "Components/CameraComponent.h"
#include "Components/TagComponent.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/CircleComponent.h"
#include "Components/CircleCollider2DComponent.h"
#include "Components/Rigidbody2DComponent.h"
#include "Components/BoxCollider2DComponent.h"
#include "NativeScriptComponent.h"

namespace Engine
{
    template <typename... T>
    struct ComponentGroup
    {
    };

    using AllComponents = ComponentGroup<
        TransformComponent,
        CameraComponent,
        SpriteComponent,
        CircleComponent,
        Rigidbody2DComponent,
        BoxCollider2DComponent,
        CircleCollider2DComponent,
        NativeScriptComponent>;
}
