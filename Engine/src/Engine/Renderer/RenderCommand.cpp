#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"
namespace Engine::Renderer
{
    RendererAPI *RenderCommand::CurrentRendererAPI = new Platform::OpenGL::OpenGLRendererAPI();

} // namespace Engine::Renderer