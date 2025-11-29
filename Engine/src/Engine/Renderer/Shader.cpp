#include "Shader.h"
#include "Engine/pcheader.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Renderer.h"
namespace Engine::Renderer
{
    Shader *Shader::Create(const std::string &vertexSrc, const std::string &fragmentSrc)
    {
        switch (Renderer::Renderer::GetAPI())
        {
        case RendererAPI::API::None:
            ENGINE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return new Platform::OpenGL::OpenGLShader(vertexSrc, fragmentSrc);
        }
        ENGINE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}