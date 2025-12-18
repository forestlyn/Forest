#include "Renderer2D.h"
#include <glm/gtc/matrix_transform.hpp>
#include "RenderCommand.h"
#include "Engine/Profile/Instrumentor.h"

namespace Engine::Renderer
{
    Renderer2D::Scene2DData Renderer2D::m_SceneData = Renderer2D::Scene2DData();

    void Renderer2D::Init()
    {
        ENGINE_PROFILING_FUNC();

        // initialize shaders
        m_SceneData.QuadTextureShader = Shader::Create("assets/shaders/TextureShader.glsl");
        // initialize vertex array

        m_SceneData.QuadVertexArray = VertexArray::Create();

        m_SceneData.QuadVertexBufferBase = new QuadVertex[m_SceneData.MaxVertices];
        m_SceneData.QuadVertexBufferPtr = m_SceneData.QuadVertexBufferBase;

        m_SceneData.QuadVertexBuffer = VertexBuffer::Create(sizeof(QuadVertex) * m_SceneData.MaxVertices);
        m_SceneData.QuadVertexBuffer->SetLayout({{ShaderDataType::Float3, "a_Position"},
                                                 {ShaderDataType::Float4, "a_Color"},
                                                 {ShaderDataType::Float2, "a_TexCoord"},
                                                 {ShaderDataType::Float, "a_TexIndex"},
                                                 {ShaderDataType::Float, "a_TilingFactor"}});
        m_SceneData.QuadVertexArray->AddVertexBuffer(m_SceneData.QuadVertexBuffer);

        uint32_t *quadIndices = new uint32_t[m_SceneData.MaxIndices];
        uint32_t offset = 0;
        for (size_t i = 0; i < m_SceneData.MaxQuads * 6; i += 6)
        {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;

            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;

            offset += 4;
        }

        Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, m_SceneData.MaxIndices);
        m_SceneData.QuadVertexArray->SetIndexBuffer(quadIB);
        delete[] quadIndices;

        // create a 1x1 white texture
        auto WhiteTexture = Texture2D::Create(1, 1);
        uint32_t whitePixel = 0xffffffff;
        WhiteTexture->SetData(&whitePixel, sizeof(uint32_t));
        m_SceneData.TextureSlots[0] = WhiteTexture;

        int32_t textureSlots[m_SceneData.MaxTextureSlots];
        for (int32_t i = 0; i < m_SceneData.MaxTextureSlots; i++)
            textureSlots[i] = i;

        m_SceneData.QuadTextureShader->Bind();
        m_SceneData.QuadTextureShader->SetIntArray("u_Textures", textureSlots, m_SceneData.MaxTextureSlots);
    }

    void Renderer2D::Shutdown()
    {
        ENGINE_PROFILING_FUNC();
    }

    void Renderer2D::BeginScene(const Camera &camera)
    {
        ENGINE_PROFILING_FUNC();

        m_SceneData.ViewProjectionMatrix = camera.GetViewProjectionMatrix();

        m_SceneData.QuadTextureShader->Bind();
        m_SceneData.QuadTextureShader->SetMat4("u_ViewProjection", m_SceneData.ViewProjectionMatrix);

        Reset();
    }

    void Renderer2D::EndScene()
    {
        ENGINE_PROFILING_FUNC();

        uint32_t dataSize = (uint32_t)((uint8_t *)m_SceneData.QuadVertexBufferPtr - (uint8_t *)m_SceneData.QuadVertexBufferBase);
        m_SceneData.QuadVertexBuffer->SetData(m_SceneData.QuadVertexBufferBase, dataSize);

        Flush();
    }

    /// @brief flush the draw calls
    void Renderer2D::Flush()
    {
        ENGINE_PROFILING_FUNC();
        for (int i = 0; i < m_SceneData.TextureSlotIndex; i++)
        {
            m_SceneData.TextureSlots[i]->Bind(i);
        }
        RenderCommand::DrawIndexed(m_SceneData.QuadVertexArray, m_SceneData.QuadIndexCount);
    }

    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color)
    {
        DrawQuad(glm::vec3(position, 0.0f), size, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color)
    {
        ENGINE_PROFILING_FUNC();

        // if exceed max index count,draw current batch and reset
        if (m_SceneData.QuadIndexCount >= m_SceneData.MaxIndices)
        {
            UploadQuadData();
            Flush();
            Reset();
        }

        const float TextureIndex = 0.0f; // White Texture
        const float TilingFactor = 1.0f;

        m_SceneData.QuadVertexBufferPtr->Position = position;
        m_SceneData.QuadVertexBufferPtr->Color = color;
        m_SceneData.QuadVertexBufferPtr->TexCoord = {0.0f, 0.0f};
        m_SceneData.QuadVertexBufferPtr->TexIndex = TextureIndex;
        m_SceneData.QuadVertexBufferPtr->TilingFactor = TilingFactor;
        m_SceneData.QuadVertexBufferPtr++;

        m_SceneData.QuadVertexBufferPtr->Position = {position.x + size.x, position.y, position.z};
        m_SceneData.QuadVertexBufferPtr->Color = color;
        m_SceneData.QuadVertexBufferPtr->TexCoord = {1.0f, 0.0f};
        m_SceneData.QuadVertexBufferPtr->TexIndex = TextureIndex;
        m_SceneData.QuadVertexBufferPtr->TilingFactor = TilingFactor;
        m_SceneData.QuadVertexBufferPtr++;

        m_SceneData.QuadVertexBufferPtr->Position = {position.x + size.x, position.y + size.y, position.z};
        m_SceneData.QuadVertexBufferPtr->Color = color;
        m_SceneData.QuadVertexBufferPtr->TexCoord = {1.0f, 1.0f};
        m_SceneData.QuadVertexBufferPtr->TexIndex = TextureIndex;
        m_SceneData.QuadVertexBufferPtr->TilingFactor = TilingFactor;
        m_SceneData.QuadVertexBufferPtr++;

        m_SceneData.QuadVertexBufferPtr->Position = {position.x, position.y + size.y, position.z};
        m_SceneData.QuadVertexBufferPtr->Color = color;
        m_SceneData.QuadVertexBufferPtr->TexCoord = {0.0f, 1.0f};
        m_SceneData.QuadVertexBufferPtr->TexIndex = TextureIndex;
        m_SceneData.QuadVertexBufferPtr->TilingFactor = TilingFactor;
        m_SceneData.QuadVertexBufferPtr++;

        m_SceneData.QuadIndexCount += 6;

        ENGINE_ASSERT(m_SceneData.QuadIndexCount <= m_SceneData.MaxIndices, "Renderer2D::DrawQuad - Exceeded max quad index count!");
    }

    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, const float tintFactor, const glm::vec4 &tintColor)
    {
        DrawQuad(glm::vec3(position, 0.0f), size, texture, tintFactor, tintColor);
    }

    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, const float tintFactor, const glm::vec4 &tintColor)
    {
        ENGINE_PROFILING_FUNC();

        if (m_SceneData.QuadIndexCount >= m_SceneData.MaxIndices)
        {
            UploadQuadData();
            Flush();
            Reset();
        }

        const float TextureIndex = GetTextureIndex(texture);

        const float TilingFactor = 1.0f;
        m_SceneData.QuadVertexBufferPtr->Position = position;
        m_SceneData.QuadVertexBufferPtr->Color = tintColor;
        m_SceneData.QuadVertexBufferPtr->TexCoord = {0.0f, 0.0f};
        m_SceneData.QuadVertexBufferPtr->TexIndex = TextureIndex;
        m_SceneData.QuadVertexBufferPtr->TilingFactor = TilingFactor;
        m_SceneData.QuadVertexBufferPtr++;

        m_SceneData.QuadVertexBufferPtr->Position = {position.x + size.x, position.y, position.z};
        m_SceneData.QuadVertexBufferPtr->Color = tintColor;
        m_SceneData.QuadVertexBufferPtr->TexCoord = {1.0f, 0.0f};
        m_SceneData.QuadVertexBufferPtr->TexIndex = TextureIndex;
        m_SceneData.QuadVertexBufferPtr->TilingFactor = TilingFactor;
        m_SceneData.QuadVertexBufferPtr++;

        m_SceneData.QuadVertexBufferPtr->Position = {position.x + size.x, position.y + size.y, position.z};
        m_SceneData.QuadVertexBufferPtr->Color = tintColor;
        m_SceneData.QuadVertexBufferPtr->TexCoord = {1.0f, 1.0f};
        m_SceneData.QuadVertexBufferPtr->TexIndex = TextureIndex;
        m_SceneData.QuadVertexBufferPtr->TilingFactor = TilingFactor;
        m_SceneData.QuadVertexBufferPtr++;

        m_SceneData.QuadVertexBufferPtr->Position = {position.x, position.y + size.y, position.z};
        m_SceneData.QuadVertexBufferPtr->Color = tintColor;
        m_SceneData.QuadVertexBufferPtr->TexCoord = {0.0f, 1.0f};
        m_SceneData.QuadVertexBufferPtr->TexIndex = TextureIndex;
        m_SceneData.QuadVertexBufferPtr->TilingFactor = TilingFactor;
        m_SceneData.QuadVertexBufferPtr++;
        m_SceneData.QuadIndexCount += 6;

        ENGINE_ASSERT(m_SceneData.QuadIndexCount <= m_SceneData.MaxIndices, "Renderer2D::DrawQuad - Exceeded max quad index count!");

        // glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
        //                       glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

        // m_SceneData.QuadTextureShader->Bind();
        // m_SceneData.QuadTextureShader->SetFloat4("u_Color", tintColor);
        // m_SceneData.QuadTextureShader->SetMat4("u_Transform", transform);
        // m_SceneData.QuadTextureShader->SetFloat("u_TintFactor", tintFactor);
        // texture->Bind(0);
        // m_SceneData.QuadTextureShader->SetInt("u_Texture", 0);

        // m_SceneData.QuadVertexArray->Bind();
        // RenderCommand::DrawIndexed(m_SceneData.QuadVertexArray);
    }

    void Renderer2D::DrawRotateQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color, const float z_rotation)
    {
        DrawRotateQuad(glm::vec3(position, 0.0f), size, color, z_rotation);
    }

    void Renderer2D::DrawRotateQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color, const float z_rotation)
    {
        ENGINE_PROFILING_FUNC();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                              glm::rotate(glm::mat4(1.0f), z_rotation, glm::vec3(0.0f, 0.0f, 1.0f)) *
                              glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

        m_SceneData.QuadTextureShader->Bind();
        m_SceneData.QuadTextureShader->SetFloat4("u_Color", color);
        m_SceneData.QuadTextureShader->SetMat4("u_Transform", transform);

        m_SceneData.QuadTextureShader->SetInt("u_Texture", 0);

        m_SceneData.QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(m_SceneData.QuadVertexArray);
    }

    void Renderer2D::DrawRotateQuad(const glm::vec2 &position, const glm::vec2 &size, const float z_rotation, const Ref<Texture2D> &texture, const float tintFactor, const glm::vec4 &tintColor)
    {
        DrawRotateQuad(glm::vec3(position, 0.0f), size, z_rotation, texture, tintFactor, tintColor);
    }

    void Renderer2D::DrawRotateQuad(const glm::vec3 &position, const glm::vec2 &size, const float z_rotation, const Ref<Texture2D> &texture, const float tintFactor, const glm::vec4 &tintColor)
    {
        ENGINE_PROFILING_FUNC();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                              glm::rotate(glm::mat4(1.0f), z_rotation, glm::vec3(0.0f, 0.0f, 1.0f)) *
                              glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

        m_SceneData.QuadTextureShader->Bind();
        m_SceneData.QuadTextureShader->SetFloat4("u_Color", tintColor);
        m_SceneData.QuadTextureShader->SetMat4("u_Transform", transform);
        m_SceneData.QuadTextureShader->SetFloat("u_TintFactor", tintFactor);
        texture->Bind(0);
        m_SceneData.QuadTextureShader->SetInt("u_Texture", 0);

        m_SceneData.QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(m_SceneData.QuadVertexArray);
    }

    float Renderer2D::GetTextureIndex(const Ref<Texture2D> &texture)
    {
        for (uint32_t i = 1; i < m_SceneData.TextureSlotIndex; i++)
        {
            if (m_SceneData.TextureSlots[i] == texture)
                return (float)i;
        }

        // New Texture
        if (m_SceneData.TextureSlotIndex >= Renderer2D::Scene2DData::MaxTextureSlots)
        {
            UploadQuadData();
            Flush();
            Reset();
        }

        m_SceneData.TextureSlots[m_SceneData.TextureSlotIndex] = texture;
        return (float)(m_SceneData.TextureSlotIndex++);
    }
    /// @brief upload the quad vertex data to gpu
    void Renderer2D::UploadQuadData()
    {
        uint32_t dataSize = (uint32_t)((uint8_t *)m_SceneData.QuadVertexBufferPtr - (uint8_t *)m_SceneData.QuadVertexBufferBase);
        m_SceneData.QuadVertexBuffer->SetData(m_SceneData.QuadVertexBufferBase, dataSize);
    }
    /// @brief reset ptr and index
    void Renderer2D::Reset()
    {
        m_SceneData.QuadVertexBufferPtr = m_SceneData.QuadVertexBufferBase;
        m_SceneData.QuadIndexCount = 0;
        m_SceneData.TextureSlotIndex = 1;
    }
}