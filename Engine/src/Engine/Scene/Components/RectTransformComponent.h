#pragma once
#include "BaseComponent.h"

namespace Engine
{
    struct RectTransformComponent : BaseComponent
    {
        glm::vec2 AnchorMin{0.5f};
        glm::vec2 AnchorMax{0.5f};
        glm::vec2 AnchoredPosition{0.0f};
        glm::vec2 SizeDelta{100.0f, 100.0f};
        glm::vec2 Pivot{0.5f};
    };

    REFLECT_COMPONENT_BEGIN(RectTransformComponent)
    REFLECT_FIELD(AnchorMin);
    REFLECT_FIELD(AnchorMax);
    REFLECT_FIELD(AnchoredPosition);
    REFLECT_FIELD(SizeDelta);
    REFLECT_FIELD(Pivot);
    REFLECT_COMPONENT_END(RectTransformComponent)
}
