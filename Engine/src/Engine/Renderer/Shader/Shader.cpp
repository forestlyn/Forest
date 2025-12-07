#include "Shader.h"
#include "Engine/pcheader.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "../Renderer.h"
namespace Engine::Renderer
{
    Ref<Shader> Shader::Create(const std::string &name, const std::string &vertexSrc, const std::string &fragmentSrc)
    {
        switch (Renderer::Renderer::GetAPI())
        {
        case RendererAPI::API::None:
            ENGINE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return Ref<Platform::OpenGL::OpenGLShader>(new Platform::OpenGL::OpenGLShader(name, vertexSrc, fragmentSrc));
        }
        ENGINE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<Shader> Shader::Create(const std::string &filepath)
    {
        switch (Renderer::Renderer::GetAPI())
        {
        case RendererAPI::API::None:
            ENGINE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return Ref<Platform::OpenGL::OpenGLShader>(new Platform::OpenGL::OpenGLShader(filepath));
        }
        ENGINE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}