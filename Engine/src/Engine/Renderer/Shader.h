#pragma once
#include <string>
namespace Engine::Renderer
{

    class Shader
    {
    public:
        Shader(const std::string &vertexSrc, const std::string &fragmentSrc);
        virtual ~Shader() = default;

        virtual void Bind();
        virtual void Unbind();

    private:
        unsigned int m_RendererID = 0;
    };

} // namespace Engine::Renderer