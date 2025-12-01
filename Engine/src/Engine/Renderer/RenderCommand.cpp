#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"
#include "Platform/DirectX12/DirectX12RendererAPI.h"
namespace Engine::Renderer
{
#if defined(FOREST_PLATFORM_WINDOWS)
#if defined(FOREST_PLATFORM_OPENGL)
    RendererAPI *RenderCommand::CurrentRendererAPI = new ::Platform::OpenGL::OpenGLRendererAPI();
#elif defined(FOREST_PLATFORM_DIRECTX12)
    RendererAPI *RenderCommand::CurrentRendererAPI = new ::Platform::DirectX12::DirectX12RendererAPI();
#endif
#else
    ENGINE_ASSERT(false, "unsupported platform!");
#endif
} // namespace Engine::Renderer