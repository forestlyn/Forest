#include "Renderer.h"
#include "RenderCommand.h"
namespace Engine::Renderer
{
    Renderer::SceneData *Renderer::m_SceneData = new Renderer::SceneData();

    void Renderer::BeginScene(const Camera &camera)
    {
        m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
    }

    void Renderer::EndScene()
    {
    }

    void Renderer::Submit(std::shared_ptr<Shader> &shader, std::shared_ptr<VertexArray> &vertexArray, glm::mat4 transform)
    {
        shader->Bind();
        shader->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
        shader->UploadUniformMat4("u_Transform", transform);
        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray);
    }

} // namespace Engine::Renderer