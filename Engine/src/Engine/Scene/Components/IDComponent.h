#pragma once
#include "Engine/Core/UUID.h"
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
} // namespace Engine