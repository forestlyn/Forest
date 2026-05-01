#pragma once
#include "Engine/Core/UUID.h"
#include "Engine/Reflection/ReflectMacro.h"
namespace Engine
{
    struct IDComponent
    {
        UUID ID;

        IDComponent() = default;
        IDComponent(const UUID &uuid)
            : ID(uuid)
        {
        }
        IDComponent(const IDComponent &other) = default;
    };

    REFLECT_TYPE_BEGIN(IDComponent)
    REFLECT_FIELD(ID);
    REFLECT_TYPE_END(IDComponent)
} // namespace Engine
