#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
enum class MetaKind
{
    Float,
    Float2,
    Float3,
    Float4,
    Int,
    Int2,
    Int3,
    Int4,
    Bool,
    String,
    Enum,
    Struct,
    UUID
};

enum PropertyFlags : uint32_t
{
    Property_None = 0,
    Property_Hidden = 1 << 0,
    Property_ReadOnly = 1 << 1,
    Property_Transient = 1 << 2, // 不参与序列化
};

inline PropertyFlags operator|(PropertyFlags a, PropertyFlags b)
{
    return static_cast<PropertyFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

struct MetaType;

struct MetaUIHint
{
    float minValue = 0.0f;
    float maxValue = 0.0f;
    float step = 0.1f;
    const char *tooltip = nullptr;
};

enum class FieldCategory
{
    Value,
    ObjectReference,
    AssetReference
};

struct MetaField
{
    const char *name = nullptr;
    const MetaType *type = nullptr;
    FieldCategory category = FieldCategory::Value;

    void *(*get)(void *) = nullptr;
    const void *(*getConst)(const void *) = nullptr;

    PropertyFlags flags = Property_None;
    MetaUIHint ui{};
};

struct MetaEnumValue
{
    const char *Name = nullptr;
    int64_t Value = 0;
};

template <typename E>
int64_t EnumToIntImpl(const void *obj)
{
    return static_cast<int64_t>(*static_cast<const E *>(obj));
}

template <typename E>
void EnumFromIntImpl(void *obj, int64_t value)
{
    *static_cast<E *>(obj) = static_cast<E>(value);
}

struct MetaType
{
    const char *name = nullptr;
    MetaKind kind = MetaKind::Struct;
    size_t size = 0;

    const std::vector<MetaField> *fields = nullptr;
    const std::vector<MetaEnumValue> *enumValues = nullptr;

    int64_t (*EnumToInt)(const void *) = nullptr;
    void (*EnumFromInt)(void *, int64_t) = nullptr;
};