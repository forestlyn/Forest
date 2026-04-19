#pragma once
#include <string>
#include "BaseComponent.h"
namespace Engine
{
    struct TagComponent : public BaseComponent
    {
    public:
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent &other) = default;
        TagComponent(const std::string &tag) : Tag(tag) {}
    };

    REFLECT_TYPE_BEGIN(TagComponent)
    REFLECT_FIELD(Tag)
    REFLECT_TYPE_END(TagComponent)
}