#include "VertexArray.h"
#include "Engine/pcheader.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Platform/DirectX12/DirectX12VertexArray.h"
namespace Engine::Renderer
{
    VertexArray::~VertexArray()
    {
    }

    VertexArray *VertexArray::Create()
    {
        switch (Renderer::Renderer::GetAPI())
        {
        case RendererAPI::API::None:
            ENGINE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return new Platform::OpenGL::OpenGLVertexArray();
        case RendererAPI::API::DirectX12:
            return new Platform::DirectX12::DirectX12VertexArray();
        }
        ENGINE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
} // namespace Engine::Renderer