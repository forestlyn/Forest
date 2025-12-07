#include "ShaderLibrary.h"

namespace Engine::Renderer
{
    void ShaderLibrary::Add(const std::string &name, Ref<Shader> shader)
    {
        ENGINE_ASSERT(!Exists(name), "Shader already exists!");
        m_Shaders[name] = shader;
    }

    void ShaderLibrary::Add(Ref<Shader> shader)
    {
        auto &name = shader->GetName();
        Add(name, shader);
    }

    Ref<Shader> ShaderLibrary::Load(const std::string &filepath)
    {
        Ref<Shader> shader = Shader::Create(filepath);
        Add(shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Load(const std::string &name, const std::string &filepath)
    {
        Ref<Shader> shader = Shader::Create(filepath);
        Add(name, shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Get(const std::string &name)
    {
        ENGINE_ASSERT(Exists(name), "Shader not found!");
        return m_Shaders[name];
    }

    bool ShaderLibrary::Exists(const std::string &name) const
    {
        return m_Shaders.find(name) != m_Shaders.end();
    }
} // namespace Engine::Renderer