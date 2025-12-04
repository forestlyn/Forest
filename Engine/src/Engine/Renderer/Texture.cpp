#include "Texture.h"
#include "Engine/pcheader.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Engine::Renderer
{
    Texture2D *Texture2D::Create(const std::string &path)
    {
        switch (Renderer::Renderer::GetAPI())
        {
        case RendererAPI::API::None:
            ENGINE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return new Platform::OpenGL::OpenGLTexture2D(path);
        default:
            break;
        }
        ENGINE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
} // namespace Engine::Renderer