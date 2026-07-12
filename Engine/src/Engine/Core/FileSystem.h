#pragma once
#include "Core.h"
#include "Buffer.h"
#include "yaml-cpp/yaml.h"
#include <filesystem>
namespace Engine::Core
{
    class FileSystem
    {
    public:
        static Buffer ReadFileAsBuffer(const std::filesystem::path &filepath);

        static YAML::Node ReadYamlFile(const std::filesystem::path &filepath);
    };
}