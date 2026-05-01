#pragma once
#include "Reflect.h"
#include <type_traits>
#include <utility>

namespace Engine
{
    namespace Detail
    {
        template <typename T>
        struct MemberPointerTraits;

        template <typename Class, typename FieldT>
        struct MemberPointerTraits<FieldT Class::*>
        {
            using ClassType = Class;
            using FieldType = FieldT;
        };
    }

    template <typename Class>
    class FieldBuilder
    {
    public:
        explicit FieldBuilder(MetaField &field)
            : m_Field(field)
        {
        }

        FieldBuilder &Flags(PropertyFlags flags)
        {
            m_Field.flags = flags;
            return *this;
        }

        FieldBuilder &Category(FieldCategory category)
        {
            m_Field.category = category;
            return *this;
        }

        FieldBuilder &UI(float minValue, float maxValue, float step)
        {
            m_Field.ui.minValue = minValue;
            m_Field.ui.maxValue = maxValue;
            m_Field.ui.step = step;
            return *this;
        }

        FieldBuilder &UI()
        {
            return *this;
        }

        FieldBuilder &UI(MetaUIHint ui)
        {
            m_Field.ui = ui;
            return *this;
        }

        FieldBuilder &Tooltip(const char *tooltip)
        {
            m_Field.ui.tooltip = tooltip;
            return *this;
        }

        FieldBuilder &TOOLTIP(const char *tooltip)
        {
            return Tooltip(tooltip);
        }

        template <bool (*Predicate)(const Class &)>
        FieldBuilder &VisibleIf()
        {
            m_Field.visible = [](const void *obj) -> bool
            {
                return Predicate(*static_cast<const Class *>(obj));
            };
            return *this;
        }

        template <bool (*Predicate)(const Class &)>
        FieldBuilder &VISIBLEIF()
        {
            return VisibleIf<Predicate>();
        }

        FieldBuilder &FLAGS(PropertyFlags flags)
        {
            return Flags(flags);
        }

        FieldBuilder &CATEGORY(FieldCategory category)
        {
            return Category(category);
        }

    private:
        MetaField &m_Field;
    };

    template <typename Class>
    class TypeBuilder
    {
    public:
        template <auto Member>
        FieldBuilder<Class> Field(const char *name)
        {
            using MemberPointer = decltype(Member);
            using Traits = Detail::MemberPointerTraits<MemberPointer>;
            using Owner = typename Traits::ClassType;
            using FieldT = typename Traits::FieldType;

            static_assert(std::is_same_v<Owner, Class>, "Reflected member belongs to a different type");

            m_Fields.push_back(MakeField<Class, FieldT, Member>(name));
            return FieldBuilder<Class>(m_Fields.back());
        }

        const std::vector<MetaField> &Fields() const
        {
            return m_Fields;
        }

    private:
        std::vector<MetaField> m_Fields;
    };

    template <typename Class, typename Fn>
    std::vector<MetaField> BuildFields(Fn &&fn)
    {
        TypeBuilder<Class> builder;
        std::forward<Fn>(fn)(builder);
        return builder.Fields();
    }
}
