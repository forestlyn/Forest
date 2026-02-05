#pragma once
#include "BaseComponent.h"
#include <string>
namespace Engine
{
    struct ScriptComponent
    {
        ScriptComponent() = default;
        ScriptComponent(const ScriptComponent &other) = default;
        std::string ScriptClassName;
    };

} // namespace Engine
