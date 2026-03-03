#include "FileSystem.h"
#include <fstream>
namespace Engine::Core
{
    Buffer FileSystem::ReadFileAsBuffer(const std::filesystem::path &filepath)
    {
        std::ifstream file(filepath, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            ENGINE_ERROR("Failed to open file: {}", filepath.string());
            return Buffer();
        }
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        Buffer buffer(size);
        if (!file.read(buffer.As<char>(), size))
        {
            ENGINE_ERROR("Failed to read file: {}", filepath.string());
            buffer.Release();
            return Buffer();
        }
        return buffer;
    }
}