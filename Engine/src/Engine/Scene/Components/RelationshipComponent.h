#pragma once
#include "BaseComponent.h"
#include "Engine/Scene/EntityRef.h"

namespace Engine
{
    struct RelationshipComponent : public BaseComponent
    {
        RelationshipComponent() = default;
        UUID GetParent() const { return Parent.uuid; }
        int GetSiblingOrder() const { return SiblingOrder; }

    private:
        EntityRef Parent;
        int SiblingOrder = 0;
        friend class Scene;
        friend struct MetaResolver<RelationshipComponent>;
    };

    REFLECT_COMPONENT_BEGIN(RelationshipComponent)
    REFLECT_FIELD(Parent).Category(FieldCategory::EntityReference).UI(MetaUIHint{.uiProperty = UIProperty::ReadOnly});
    REFLECT_FIELD(SiblingOrder).UI(MetaUIHint{.uiProperty = UIProperty::ReadOnly});
    REFLECT_COMPONENT_END(RelationshipComponent)
} // namespace Engine
