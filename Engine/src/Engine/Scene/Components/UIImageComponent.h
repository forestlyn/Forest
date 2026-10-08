#pragma once
#include "BaseComponent.h"
#include "Engine/Resource/ResourceRef.h"
#include "Engine/Renderer/Shader/Texture.h"

namespace Engine
{
    struct UIImageComponent : BaseComponent
    {
        ResourceRef<Renderer::Texture2D> TextureRef;
        glm::vec4 Color{1.0f};
        // Reserved for P4 hit testing; this does not capture input in P2.
        bool RaycastTarget = true;
    };

    REFLECT_COMPONENT_BEGIN(UIImageComponent)
    REFLECT_FIELD(TextureRef).Category(FieldCategory::AssetReference).UIKIND(UIKind::UITYPE_Texture2D);
    REFLECT_FIELD(Color).UIKIND(UIKind::UITYPE_Color);
    REFLECT_FIELD(RaycastTarget);
    REFLECT_COMPONENT_END(UIImageComponent)
}
