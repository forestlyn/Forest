#pragma once
#include "MetaStruct.h"
#include "Engine/Core/UUID.h"
template <typename T>
struct MetaResolver;

template <typename T>
const MetaType &Reflect()
{
    return MetaResolver<T>::Get();
}

template <typename Class, typename FieldT, FieldT Class::*Member>
void *MetaGet(void *obj)
{
    return &(static_cast<Class *>(obj)->*Member);
}

template <typename Class, typename FieldT, FieldT Class::*Member>
const void *MetaGetConst(const void *obj)
{
    return &(static_cast<const Class *>(obj)->*Member);
}

template <typename Class, typename FieldT, FieldT Class::*Member>
MetaField MakeField(const char *name,
                    PropertyFlags flags = Property_None,
                    MetaUIHint ui = {})
{
    return MetaField{
        name,
        &Reflect<FieldT>(),
        &MetaGet<Class, FieldT, Member>,
        &MetaGetConst<Class, FieldT, Member>,
        flags,
        ui};
}

template <>
struct MetaResolver<float>
{
    static const MetaType &Get()
    {
        static const MetaType t{"float", MetaKind::Float, sizeof(float), nullptr};
        return t;
    }
};

template <>
struct MetaResolver<glm::vec2>
{
    static const MetaType &Get()
    {
        static const std::vector<MetaField> fields =
            {
                MakeField<glm::vec2, float, &glm::vec2::x>("x"),
                MakeField<glm::vec2, float, &glm::vec2::y>("y")};
        static const MetaType t{"float2", MetaKind::Float2, sizeof(glm::vec2), &fields};
        return t;
    }
};

template <>
struct MetaResolver<glm::vec3>
{
    static const MetaType &Get()
    {
        static const std::vector<MetaField> fields =
            {
                MakeField<glm::vec3, float, &glm::vec3::x>("x"),
                MakeField<glm::vec3, float, &glm::vec3::y>("y"),
                MakeField<glm::vec3, float, &glm::vec3::z>("z"),
            };
        static const MetaType t{"float3", MetaKind::Float3, sizeof(glm::vec3), &fields};
        return t;
    }
};

template <>
struct MetaResolver<glm::vec4>
{
    static const MetaType &Get()
    {
        static const std::vector<MetaField> fields =
            {
                MakeField<glm::vec4, float, &glm::vec4::x>("x"),
                MakeField<glm::vec4, float, &glm::vec4::y>("y"),
                MakeField<glm::vec4, float, &glm::vec4::z>("z"),
                MakeField<glm::vec4, float, &glm::vec4::w>("w"),
            };
        static const MetaType t{"float4", MetaKind::Float4, sizeof(glm::vec4), &fields};
        return t;
    }
};

template <>
struct MetaResolver<int>
{
    static const MetaType &Get()
    {
        static const MetaType t{"int", MetaKind::Int, sizeof(int), nullptr};
        return t;
    }
};

template <>
struct MetaResolver<glm::ivec2>
{
    static const MetaType &Get()
    {
        static const std::vector<MetaField> fields =
            {
                MakeField<glm::ivec2, int, &glm::ivec2::x>("x"),
                MakeField<glm::ivec2, int, &glm::ivec2::y>("y")};
        static const MetaType t{"int2", MetaKind::Int2, sizeof(glm::ivec2), &fields};
        return t;
    }
};

template <>
struct MetaResolver<glm::ivec3>
{
    static const MetaType &Get()
    {
        static const std::vector<MetaField> fields =
            {
                MakeField<glm::ivec3, int, &glm::ivec3::x>("x"),
                MakeField<glm::ivec3, int, &glm::ivec3::y>("y"),
                MakeField<glm::ivec3, int, &glm::ivec3::z>("z"),
            };
        static const MetaType t{"int3", MetaKind::Int3, sizeof(glm::ivec3), &fields};
        return t;
    }
};

template <>
struct MetaResolver<glm::ivec4>
{
    static const MetaType &Get()
    {
        static const std::vector<MetaField> fields =
            {
                MakeField<glm::ivec4, int, &glm::ivec4::x>("x"),
                MakeField<glm::ivec4, int, &glm::ivec4::y>("y"),
                MakeField<glm::ivec4, int, &glm::ivec4::z>("z"),
                MakeField<glm::ivec4, int, &glm::ivec4::w>("w"),
            };
        static const MetaType t{"int4", MetaKind::Int4, sizeof(glm::ivec4), &fields};
        return t;
    }
};

template <>
struct MetaResolver<bool>
{
    static const MetaType &Get()
    {
        static const MetaType t{"bool", MetaKind::Bool, sizeof(bool), nullptr};
        return t;
    }
};

template <>
struct MetaResolver<std::string>
{
    static const MetaType &Get()
    {
        static const MetaType t{"string", MetaKind::String, sizeof(std::string), nullptr};
        return t;
    }
};

template <>
struct MetaResolver<Engine::UUID>
{
    static const MetaType &Get()
    {
        static const MetaType t{"uuid", MetaKind::UUID, sizeof(Engine::UUID), nullptr};
        return t;
    }
};