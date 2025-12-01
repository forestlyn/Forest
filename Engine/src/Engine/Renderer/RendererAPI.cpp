#include "RendererAPI.h"

namespace Engine::Renderer
{
#if defined(FOREST_PLATFORM_WINDOWS)
#if defined(FOREST_PLATFORM_OPENGL)
    RendererAPI::API RendererAPI::CurrentAPI = RendererAPI::API::OpenGL;
#elif defined(FOREST_PLATFORM_DIRECTX12)
    RendererAPI::API RendererAPI::CurrentAPI = RendererAPI::API::DirectX12;
#endif
#else
    RendererAPI::API RendererAPI::CurrentAPI = RendererAPI::API::None;
#endif
}