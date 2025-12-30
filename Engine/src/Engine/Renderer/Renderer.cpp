#include "Renderer.h"
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Renderer2D.h"
#include "Engine/Profile/Instrumentor.h"
namespace Engine::Renderer
{
    Renderer::SceneData *Renderer::m_SceneData = new Renderer::SceneData();

    void Renderer::Init()
    {
        ENGINE_PROFILING_FUNC();
        RenderCommand::Init();
        Renderer2D::Init();
    }

    void Renderer::Shutdown()
    {
        ENGINE_PROFILING_FUNC();
        Renderer2D::Shutdown();
    }

    void Renderer::BeginScene(glm::mat4 viewProjectionMatrix)
    {
        m_SceneData->ViewProjectionMatrix = viewProjectionMatrix;
    }

    void Renderer::EndScene()
    {
    }

    void Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        RenderCommand::SetViewport(x, y, width, height);
    }

    void Renderer::Submit(Ref<Shader> &shader, Ref<VertexArray> &vertexArray, glm::mat4 transform)
    {
        shader->Bind();
        shader->SetMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
        shader->SetMat4("u_Transform", transform);
        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray);
    }

} // namespace Engine::Renderer