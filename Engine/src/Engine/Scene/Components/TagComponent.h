#pragma once
#include <string>

namespace Engine
{
    struct TagComponent
    {
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent &other) = delete;
        TagComponent(const std::string &tag) : Tag(tag) {}
    };
}