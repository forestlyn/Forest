#include "FrameBuffer.h"
#include "Engine/pcheader.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"
namespace Engine::Renderer
{
    Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification &spec)
    {
        switch (Renderer::Renderer::GetAPI())
        {
        case RendererAPI::API::None:
            ENGINE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return Ref<Platform::OpenGL::OpenGLFrameBuffer>(new Platform::OpenGL::OpenGLFrameBuffer(spec));
        }
        ENGINE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
} // namespace Engine::Renderer