#include "Renderer2D.h"
#include <glm/gtc/matrix_transform.hpp>
#include "RenderCommand.h"
#include "Engine/Profile/Instrumentor.h"

namespace Engine::Renderer
{
    Renderer2D::Scene2DData *Renderer2D::m_SceneData = new Renderer2D::Scene2DData();

    void Renderer2D::Init()
    {
        // initialize shaders
        m_SceneData->QuadTextureShader = Shader::Create("assets/shaders/TextureShader.glsl");
        // initialize vertex array
        float quadVertices[5 * 4] = {
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f};

        m_SceneData->QuadVertexArray = VertexArray::Create();

        Ref<VertexBuffer> quadVB = VertexBuffer::Create(quadVertices, sizeof(quadVertices));
        quadVB->SetLayout({{ShaderDataType::Float3, "a_Position"},
                           {ShaderDataType::Float2, "a_TexCoord"}});
        m_SceneData->QuadVertexArray->AddVertexBuffer(quadVB);

        uint32_t quadIndices[6] = {0, 1, 2, 2, 3, 0};
        Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, sizeof(quadIndices) / sizeof(uint32_t));
        m_SceneData->QuadVertexArray->SetIndexBuffer(quadIB);

        // create a 1x1 white texture
        m_SceneData->WhiteTexture = Texture2D::Create(1, 1);
        uint32_t whitePixel = 0xffffffff;
        m_SceneData->WhiteTexture->SetData(&whitePixel, sizeof(uint32_t));
    }

    void Renderer2D::Shutdown()
    {
        ENGINE_PROFILING_FUNC();
        delete m_SceneData;
    }

    void Renderer2D::BeginScene(const Camera &camera)
    {
        m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();

        m_SceneData->QuadTextureShader->Bind();
        m_SceneData->QuadTextureShader->SetMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
    }

    void Renderer2D::EndScene()
    {
    }

    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color)
    {
        DrawQuad(glm::vec3(position, 0.0f), size, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                              glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

        m_SceneData->QuadTextureShader->Bind();
        m_SceneData->QuadTextureShader->SetMat4("u_Transform", transform);
        m_SceneData->QuadTextureShader->SetFloat4("u_Color", color);

        m_SceneData->WhiteTexture->Bind(0);
        m_SceneData->QuadTextureShader->SetInt("u_Texture", 0);
        m_SceneData->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(m_SceneData->QuadVertexArray);
    }

    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<Texture2D> &texture)
    {
        DrawQuad(glm::vec3(position, 0.0f), size, texture);
    }

    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                              glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

        m_SceneData->QuadTextureShader->Bind();
        m_SceneData->QuadTextureShader->SetFloat4("u_Color", glm::vec4(1.0f));
        m_SceneData->QuadTextureShader->SetMat4("u_Transform", transform);
        texture->Bind(0);
        m_SceneData->QuadTextureShader->SetInt("u_Texture", 0);

        m_SceneData->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(m_SceneData->QuadVertexArray);
    }
    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const float z_rotation = 0, const Ref<Texture2D> &texture = nullptr, const glm::vec4 &tintColor = glm::vec4(1.0f))
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(z_rotation), glm::vec3(0.0f, 0.0f, 1.0f)) *
                              glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

        m_SceneData->QuadTextureShader->Bind();
        m_SceneData->QuadTextureShader->SetFloat4("u_Color", tintColor);
        m_SceneData->QuadTextureShader->SetMat4("u_Transform", transform);
        texture ? texture->Bind(0) : m_SceneData->WhiteTexture->Bind(0);
        m_SceneData->QuadTextureShader->SetInt("u_Texture", 0);
        m_SceneData->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(m_SceneData->QuadVertexArray);
    }
}