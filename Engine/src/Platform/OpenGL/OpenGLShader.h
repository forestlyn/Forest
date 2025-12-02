#pragma once
#include "Engine/Renderer/Shader.h"
#include "Engine/pcheader.h"
#include <glm/glm.hpp>
namespace Platform::OpenGL
{

    class OpenGLShader : public Engine::Renderer::Shader
    {
    public:
        OpenGLShader(const std::string &vertexSrc, const std::string &fragmentSrc);
        virtual ~OpenGLShader();
        virtual void Bind() const override;
        virtual void Unbind() const override;
        virtual void UploadUniformMat4(const std::string &name, const glm::mat4 &matrix) override;

    private:
        uint32_t m_RendererID = 0;
    };

} // namespace Engine::Renderer