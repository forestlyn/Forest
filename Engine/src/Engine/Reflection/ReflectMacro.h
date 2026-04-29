#pragma once
#include "Reflect.h"

namespace Engine
{
#define REFLECT_TYPE_BEGIN(Type)     \
    template <>                      \
    struct MetaResolver<Type>        \
    {                                \
        static const MetaType &Get() \
        {                            \
            using Self = Type;       \
            static const std::vector<MetaField> fields = {

#define REFLECT_FIELD(Member) \
    MakeField<Self, decltype(Self::Member), &Self::Member>(#Member),
#define REFLECT_FIELD_FLAGS(Member, Flags) \
    MakeField<Self, decltype(Self::Member), &Self::Member>(#Member, Flags),
#define REFLECT_FIELD_UI(Member, Flags, MinV, MaxV, StepV)  \
    MakeField<Self, decltype(Self::Member), &Self::Member>( \
        #Member, Flags, MetaUIHint{MinV, MaxV, StepV, nullptr}),
#define REFLECT_FIELD_UI_TOOLTIP(Member, Flags, MinV, MaxV, StepV, Tooltip) \
    MakeField<Self, decltype(Self::Member), &Self::Member>(                 \
        #Member, Flags, MetaUIHint{MinV, MaxV, StepV, Tooltip}),

#define REFLECT_TYPE_END(Type)                                               \
    }                                                                        \
    ;                                                                        \
    static const MetaType t{#Type, MetaKind::Struct, sizeof(Self), &fields}; \
    return t;                                                                \
    }                                                                        \
    }                                                                        \
    ;

#define REFLECT_ENUM_BEGIN(EnumType) \
    template <>                      \
    struct MetaResolver<EnumType>    \
    {                                \
        static const MetaType &Get() \
        {                            \
            using Self = EnumType;   \
            static const std::vector<MetaEnumValue> values = {

#define REFLECT_ENUM_VALUE(Value) \
    {#Value, static_cast<int64_t>(Self::Value)},

#define REFLECT_ENUM_END(EnumType)                                                                                                            \
    }                                                                                                                                         \
    ;                                                                                                                                         \
    static const MetaType t{#EnumType, MetaKind::Enum, sizeof(Self), nullptr, &values, &EnumToIntImpl<EnumType>, &EnumFromIntImpl<EnumType>}; \
    return t;                                                                                                                                 \
    }                                                                                                                                         \
    }                                                                                                                                         \
    ;
}