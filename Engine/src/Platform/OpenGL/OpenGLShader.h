#pragma once
#include "Engine/Renderer/Shader/Shader.h"
#include "Engine/pcheader.h"
#include <glm/glm.hpp>
#include <unordered_map>

typedef unsigned int GLenum;
namespace Platform::OpenGL
{

    class OpenGLShader : public Engine::Renderer::Shader
    {
    public:
        OpenGLShader(const std::string &name, const std::string &vertexSrc, const std::string &fragmentSrc);
        OpenGLShader(const std::string &filepath);
        virtual ~OpenGLShader();
        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual const std::string &GetName() const override { return m_Name; }

        virtual void SetInt(const std::string &name, int value) override;
        virtual void SetFloat3(const std::string &name, const glm::vec3 &value) override;
        virtual void SetFloat4(const std::string &name, const glm::vec4 &value) override;
        virtual void SetMat4(const std::string &name, const glm::mat4 &matrix) override;

        virtual void UploadUniformInt(const std::string &name, int value);

        virtual void UploadUniformFloat(const std::string &name, float value);
        virtual void UploadUniformFloat2(const std::string &name, const glm::vec2 &value);
        virtual void UploadUniformFloat3(const std::string &name, const glm::vec3 &value);
        virtual void UploadUniformFloat4(const std::string &name, const glm::vec4 &value);

        virtual void UploadUniformMat3(const std::string &name, const glm::mat3 &matrix);
        virtual void UploadUniformMat4(const std::string &name, const glm::mat4 &matrix);

    private:
        std::unordered_map<GLenum, std::string> PreProcess(const std::string &source);

        void CompileShader(std::unordered_map<GLenum, std::string> &shaderSources);

        void ExtractNameFromFilepath(const std::string &filepath);

    private:
        uint32_t m_RendererID = 0;
        std::string m_Name;
    };

} // namespace Engine::Renderer