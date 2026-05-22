#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace YAML
{
    class Emitter;
    class Node;
}

namespace Engine
{

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
        UUID,
        EntityRef,
        ResourceRef
    };

    enum PropertyFlags : uint32_t
    {
        Property_None = 0, // 默认属性，无特殊标记

        // 序列化相关
        Property_Serializable = 1 << 3, // 参与序列化（默认）
        Property_Transient = 1 << 4,    // 不参与序列化
    };

    enum UIKind
    {
        UITYPE_Default,
        UITYPE_Color,
        UITYPE_FilePath,
        UITYPE_Texture2D,
    };

    enum UIProperty
    {
        Editable,
        ReadOnly,
        Hidden,
    };

    inline PropertyFlags operator|(PropertyFlags a, PropertyFlags b)
    {
        return static_cast<PropertyFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline bool HasPropertyFlag(PropertyFlags flags, PropertyFlags flag)
    {
        return (static_cast<uint32_t>(flags) & static_cast<uint32_t>(flag)) != 0;
    }

    struct MetaType;

    struct MetaUIHint
    {
        bool hasMin = false;
        float minValue = 0.0f;
        bool hasMax = false;
        float maxValue = 0.0f;
        float step = 0.1f;
        const char *tooltip = nullptr;
        UIKind uiKind = UIKind::UITYPE_Default;
        UIProperty uiProperty = UIProperty::Editable;
    };

    enum class FieldCategory
    {
        Value,
        EntityReference,
        AssetReference
    };

    struct MetaField
    {
        const char *name = nullptr;
        const MetaType *type = nullptr;
        FieldCategory category = FieldCategory::Value;

        void *(*get)(void *) = nullptr;
        const void *(*getConst)(const void *) = nullptr;

        PropertyFlags flags = PropertyFlags::Property_Serializable;

        // UI 提示信息
        MetaUIHint ui{};
        bool (*visible)(const void *) = nullptr;
        void (*onChanged)(void *) = nullptr;
    };

    inline bool IsFieldVisible(const MetaField &field, const void *obj)
    {
        return !field.visible || field.visible(obj);
    }

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

        void (*SerializeYaml)(YAML::Emitter &, const void *) = nullptr;
        bool (*DeserializeYaml)(void *, const YAML::Node &) = nullptr;
    };
}
