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

    REFLECT_COMPONENT_BEGIN(ScriptComponent)
    REFLECT_FIELD(ScriptClassName);
    REFLECT_COMPONENT_END(ScriptComponent)

} // namespace Engine
