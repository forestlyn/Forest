#include "UniformBuffer.h"
#include "Engine/pcheader.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"
namespace Engine::Renderer
{
    Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
    {
        switch (Renderer::Renderer::GetAPI())
        {
        case RendererAPI::API::None:
            ENGINE_ASSERT(false, "Renderer API None is currently not supported!");
            break;
        case RendererAPI::API::OpenGL:
            return Ref<Platform::OpenGL::OpenGLUniformBuffer>(new Platform::OpenGL::OpenGLUniformBuffer(size, binding));
        default:
            break;
        }
        ENGINE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }
}