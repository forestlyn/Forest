#include "Renderer2D.h"
#include <glm/gtc/matrix_transform.hpp>
#include "RenderCommand.h"

namespace Engine::Renderer
{
    Renderer2D::Scene2DData *Renderer2D::m_SceneData = new Renderer2D::Scene2DData();

    void Renderer2D::Init()
    {
        m_SceneData->QuadShader = Shader::Create("assets/shaders/FlatColorShader.glsl");

        float quadVertices[3 * 4] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.5f, 0.5f, 0.0f,
            -0.5f, 0.5f, 0.0f};

        m_SceneData->QuadVertexArray = VertexArray::Create();

        Ref<VertexBuffer> quadVB = VertexBuffer::Create(quadVertices, sizeof(quadVertices));
        quadVB->SetLayout({{ShaderDataType::Float3, "a_Position"}});
        m_SceneData->QuadVertexArray->AddVertexBuffer(quadVB);

        uint32_t quadIndices[6] = {0, 1, 2, 2, 3, 0};
        Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, sizeof(quadIndices) / sizeof(uint32_t));
        m_SceneData->QuadVertexArray->SetIndexBuffer(quadIB);
    }

    void Renderer2D::Shutdown()
    {
    }

    void Renderer2D::BeginScene(const Camera &camera)
    {
        m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();

        m_SceneData->QuadShader->Bind();
        m_SceneData->QuadShader->SetMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
    }

    void Renderer2D::EndScene()
    {
    }

    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color)
    {

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f)) *
                              glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

        m_SceneData->QuadShader->SetMat4("u_Transform", transform);
        m_SceneData->QuadShader->SetFloat4("u_Color", color);

        m_SceneData->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(m_SceneData->QuadVertexArray);
    }
}