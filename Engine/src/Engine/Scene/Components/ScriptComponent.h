#pragma once
#include "BaseComponent.h"
#include <string>
namespace Engine
{
    struct ScriptComponent : public BaseComponent
    {
        ScriptComponent() = default;
        ScriptComponent(const ScriptComponent &other) = default;
        std::string ScriptClassName;
    };

} // namespace Engine
