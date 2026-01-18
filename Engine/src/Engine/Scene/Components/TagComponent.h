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
        TagComponent(const TagComponent &other) = delete;
        TagComponent(const std::string &tag) : Tag(tag) {}
    };
}