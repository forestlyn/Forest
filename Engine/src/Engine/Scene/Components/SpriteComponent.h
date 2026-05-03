#pragma once
#include <glm/glm.hpp>
#include "BaseComponent.h"
#include "Engine/Renderer/Shader/Texture.h"
#include "Engine/Resource/ResourceRef.h"
#include "Engine/Resource/ResourceManager.h"
namespace Engine
{
    struct SpriteComponent : public BaseComponent
    {
        glm::vec4 Color{1.0f};
        float TilingFactor = 1.0f;
        Engine::ResourceRef<Engine::Renderer::Texture2D> TextureRef;

        SpriteComponent() = default;
        SpriteComponent(const SpriteComponent &other) = default;
        SpriteComponent(const glm::vec4 &color) : Color(color) {}
    };

    REFLECT_TYPE_BEGIN(SpriteComponent)
    REFLECT_FIELD(Color).UIKIND(UIKind::UITYPE_Color);
    REFLECT_FIELD(TilingFactor);
    REFLECT_FIELD(TextureRef).Category(FieldCategory::AssetReference).UIKIND(UIKind::UITYPE_Texture2D);
    REFLECT_TYPE_END(SpriteComponent)
}
