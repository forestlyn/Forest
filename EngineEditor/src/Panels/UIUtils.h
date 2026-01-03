#pragma once
#include <string>
#include <glm/glm.hpp>
namespace EngineEditor
{
    class UIUtils
    {
    public:
        static bool DrawVector3Control(const std::string &label, glm::vec3 &values, float resetValue = 0.0f, float columnWidth = 100.0f);
    };
}