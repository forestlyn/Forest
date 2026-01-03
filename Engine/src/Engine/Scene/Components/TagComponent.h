#pragma once
#include <string>
#include "BaseComponent.h"
namespace Engine
{
    struct TagComponent : public BaseComponent
    {
    private:
        bool m_Remove = false;
        bool m_Active = true;

    public:
        std::string Tag;

        bool IsRemove() const { return m_Remove; }
        void SetRemove(bool removed) { m_Remove = removed; }
        bool IsActive() const { return m_Active; }
        void SetActive(bool active) { m_Active = active; }

        TagComponent() = default;
        TagComponent(const TagComponent &other) = delete;
        TagComponent(const std::string &tag) : Tag(tag) {}
    };
}