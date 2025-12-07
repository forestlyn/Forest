#include "Buffer.h"
#include "Engine/pcheader.h"
#include "../Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"
namespace Engine::Renderer
{
    Ref<VertexBuffer> VertexBuffer::Create(float *vertices, uint32_t size)
    {
        switch (Renderer::Renderer::GetAPI())
        {
        case RendererAPI::API::None:
            ENGINE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return Ref<Platform::OpenGL::OpenGLVertexBuffer>(new Platform::OpenGL::OpenGLVertexBuffer(vertices, size));
        }
        ENGINE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<IndexBuffer> IndexBuffer::Create(uint32_t *indices, uint32_t count)
    {
        switch (Renderer::Renderer::GetAPI())
        {
        case RendererAPI::API::None:
            ENGINE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return Ref<Platform::OpenGL::OpenGLIndexBuffer>(new Platform::OpenGL::OpenGLIndexBuffer(indices, count));
        }
        ENGINE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
} // namespace Engine::Renderer