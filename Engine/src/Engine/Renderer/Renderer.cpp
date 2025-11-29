#include "Renderer.h"
#include "RenderCommand.h"
namespace Engine::Renderer
{
    void Renderer::BeginScene()
    {
    }

    void Renderer::EndScene()
    {
    }

    void Renderer::Submit(std::shared_ptr<VertexArray> &vertexArray)
    {
        RenderCommand::Submit(vertexArray);
    }

} // namespace Engine::Renderer