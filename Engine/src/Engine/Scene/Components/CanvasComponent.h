#pragma once
#include "BaseComponent.h"

namespace Engine
{
    enum class CanvasScaleMode
    {
        ConstantPixelSize,
        ScaleWithScreenSize
    };

    struct CanvasComponent : BaseComponent
    {
        glm::vec2 ReferenceResolution{1280.0f, 720.0f};
        CanvasScaleMode ScaleMode = CanvasScaleMode::ScaleWithScreenSize;
        float MatchWidthOrHeight = 0.5f;
        int SortOrder = 0;
    };

    REFLECT_ENUM_BEGIN(CanvasScaleMode)
    REFLECT_ENUM_VALUE(ConstantPixelSize)
    REFLECT_ENUM_VALUE(ScaleWithScreenSize)
    REFLECT_ENUM_END(CanvasScaleMode)

    REFLECT_COMPONENT_BEGIN(CanvasComponent)
    REFLECT_FIELD(ReferenceResolution);
    REFLECT_FIELD(ScaleMode);
    REFLECT_FIELD(MatchWidthOrHeight).UIRANGE(0.0f, 1.0f, 0.01f);
    REFLECT_FIELD(SortOrder);
    type.template Field<&Self::m_Enabled>("Enabled");
    REFLECT_COMPONENT_END(CanvasComponent)
}
