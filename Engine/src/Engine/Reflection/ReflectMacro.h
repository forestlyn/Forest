#pragma once
#include "Builder.h"

namespace Engine
{
#define REFLECT_TYPE_BEGIN(Type)     \
    template <>                      \
    struct MetaResolver<Type>        \
    {                                \
        static const MetaType &Get() \
        {                            \
            using Self = Type;       \
            static const std::vector<MetaField> fields = BuildFields<Self>([](auto &type) {

#define REFLECT_FIELD(Member) \
    type.Field<&Self::Member>(#Member)

#define REFLECT_TYPE_END(Type)                                               \
    });                                                                      \
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
