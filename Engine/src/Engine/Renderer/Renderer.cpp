#include "Renderer.h"
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLShader.h"
namespace Engine::Renderer
{
    Renderer::SceneData *Renderer::m_SceneData = new Renderer::SceneData();

    void Renderer::Init()
    {
        RenderCommand::Init();
    }

    void Renderer::BeginScene(const Camera &camera)
    {
        m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
    }

    void Renderer::EndScene()
    {
    }

    void Renderer::Submit(Ref<Shader> &shader, Ref<VertexArray> &vertexArray, glm::mat4 transform)
    {
        shader->Bind();
        dynamic_cast<Platform::OpenGL::OpenGLShader *>(shader.get())->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
        dynamic_cast<Platform::OpenGL::OpenGLShader *>(shader.get())->UploadUniformMat4("u_Transform", transform);
        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray);
    }

} // namespace Engine::Renderer