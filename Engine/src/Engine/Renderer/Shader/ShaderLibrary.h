#pragma once

#include <unordered_map>
#include "Shader.h"

namespace Engine::Renderer
{
    class ShaderLibrary
    {
    public:
        void Add(const std::string &name, Ref<Shader> shader);
        void Add(Ref<Shader> shader);
        Ref<Shader> Load(const std::string &filepath);
        Ref<Shader> Load(const std::string &name, const std::string &filepath);

        Ref<Shader> Get(const std::string &name);

        bool Exists(const std::string &name) const;

    private:
        std::unordered_map<std::string, Ref<Shader>> m_Shaders;
    };
} // namespace Engine::Renderer