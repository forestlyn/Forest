#pragma once
#include "Reflect.h"
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

#define REFLECT_FIELD_UI(Member, Flags, MinV, MaxV, StepV)  \
    MakeField<Self, decltype(Self::Member), &Self::Member>( \
        #Member, Flags, MetaUIHint{MinV, MaxV, StepV, nullptr}),

#define REFLECT_TYPE_END(Type)                                               \
    }                                                                        \
    ;                                                                        \
    static const MetaType t{#Type, MetaKind::Struct, sizeof(Self), &fields}; \
    return t;                                                                \
    }                                                                        \
    }                                                                        \
    ;