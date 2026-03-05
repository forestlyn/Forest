#pragma once
#include <filesystem>
#include "Engine.h"

namespace EngineEditor
{
    class Utils
    {
    public:
        static std::string ExtraNameFromPath(std::filesystem::path &path);
    };
}