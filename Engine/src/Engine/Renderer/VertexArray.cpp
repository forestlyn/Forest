#include "VertexArray.h"
#include "Engine/pcheader.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Engine::Renderer
{
    VertexArray::~VertexArray()
    {
    }

    VertexArray *VertexArray::Create()
    {
        switch (Renderer::Renderer::GetAPI())
        {
        case RendererAPI::None:
            ENGINE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::OpenGL:
            return new Platform::OpenGL::OpenGLVertexArray();
        }
        ENGINE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
} // namespace Engine::Renderer