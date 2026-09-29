#pragma once
#include "BaseComponent.h"
#include "Engine/Scene/EntityRef.h"

namespace Engine
{
    struct RectTransformComponent : BaseComponent
    {
        EntityRef Parent;
        int SiblingOrder = 0;
        glm::vec2 AnchorMin{0.5f};
        glm::vec2 AnchorMax{0.5f};
        glm::vec2 AnchoredPosition{0.0f};
        glm::vec2 SizeDelta{100.0f, 100.0f};
        glm::vec2 Pivot{0.5f};
    };

    REFLECT_TYPE_BEGIN(RectTransformComponent)
        REFLECT_FIELD(Parent).Category(FieldCategory::EntityReference);
        REFLECT_FIELD(SiblingOrder);
        REFLECT_FIELD(AnchorMin);
        REFLECT_FIELD(AnchorMax);
        REFLECT_FIELD(AnchoredPosition);
        REFLECT_FIELD(SizeDelta);
        REFLECT_FIELD(Pivot);
        type.template Field<&Self::m_Enabled>("Enabled");
    REFLECT_TYPE_END(RectTransformComponent)
}
