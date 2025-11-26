#pragma once
#include "Engine/Renderer/Shader.h"
#include "Engine/pcheader.h"

namespace Platform::OpenGL
{

    class OpenGLShader : public Engine::Renderer::Shader
    {
    public:
        OpenGLShader(const std::string &vertexSrc, const std::string &fragmentSrc);
        virtual ~OpenGLShader();
        virtual void Bind() const override;
        virtual void Unbind() const override;

    private:
        uint32_t m_RendererID = 0;
    };

} // namespace Engine::Renderer