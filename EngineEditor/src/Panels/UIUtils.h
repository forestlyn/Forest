#pragma once
#include <string>
#include <glm/glm.hpp>
#include <functional>
#include "Engine/Scene/Entity.h"
namespace EngineEditor
{
    class UIUtils
    {
    public:
        static bool DrawVector3Control(const std::string &label, glm::vec3 &values, float resetValue = 0.0f, float columnWidth = 100.0f);
        template <typename T>
        static void DrawComponent(const std::string &name, Engine::Entity entity, const std::function<void(T &)> &uiFunction, bool removeable = true);
    };
}