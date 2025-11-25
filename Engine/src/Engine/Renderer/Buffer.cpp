#include "Buffer.h"
#include "Engine/pcheader.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"
namespace Engine::Renderer
{
    VertexBuffer *VertexBuffer::Create(float *vertices, unsigned int size)
    {
        switch (Renderer::Renderer::GetAPI())
        {
        case RendererAPI::None:
            ENGINE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::OpenGL:
            return new Platform::OpenGL::OpenGLVertexBuffer(vertices, size);
        }

        return nullptr; // Placeholder
    }

    IndexBuffer *IndexBuffer::Create(unsigned int *indices, unsigned int count)
    {
        switch (Renderer::Renderer::GetAPI())
        {
        case RendererAPI::None:
            ENGINE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::OpenGL:
            return new Platform::OpenGL::OpenGLIndexBuffer(indices, count);
        }
        return nullptr; // Placeholder
    }
} // namespace Engine::Renderer