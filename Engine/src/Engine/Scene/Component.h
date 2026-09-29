#pragma once
#include "Components/IDComponent.h"
#include "Components/CameraComponent.h"
#include "Components/TagComponent.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/SpriteAnimationComponent.h"
#include "Components/CircleComponent.h"
#include "Components/CircleCollider2DComponent.h"
#include "Components/Rigidbody2DComponent.h"
#include "Components/BoxCollider2DComponent.h"
#include "Components/ScriptComponent.h"
#include "NativeScriptComponent.h"
#include "Components/CanvasComponent.h"
#include "Components/RectTransformComponent.h"
#include "Components/UIImageComponent.h"

namespace Engine
{
    template <typename... T>
    struct ComponentGroup
    {
    };

    template <typename T, typename... Components>
    struct IsInComponentGroupImpl : std::disjunction<std::is_same<T, Components>...>
    {
    };

    template <typename T, typename Group>
    struct IsInComponentGroupHelper;

    template <typename T, typename... Components>
    struct IsInComponentGroupHelper<T, ComponentGroup<Components...>>
        : IsInComponentGroupImpl<T, Components...>
    {
    };

    template <typename T, typename Group>
    concept IsInComponentGroup = IsInComponentGroupHelper<T, Group>::value;

    using AllComponents = ComponentGroup<
        UIImageComponent,
        CanvasComponent,
        RectTransformComponent,
        TransformComponent,
        CameraComponent,
        SpriteComponent,
        CircleComponent,
        Rigidbody2DComponent,
        BoxCollider2DComponent,
        CircleCollider2DComponent,
        ScriptComponent,
        SpriteAnimationComponent,
        NativeScriptComponent>;

    // Only types that have a ScriptCore managed wrapper belong here.
    using ScriptableComponents = ComponentGroup<TransformComponent, Rigidbody2DComponent>;

    // UI creation needs a valid parent/root and will use dedicated editor commands in P5.
    using AddableComponents = ComponentGroup<
        CameraComponent,
        SpriteComponent,
        SpriteAnimationComponent,
        CircleComponent,
        Rigidbody2DComponent,
        BoxCollider2DComponent,
        CircleCollider2DComponent>;
}
