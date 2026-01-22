#include "Renderer2D.h"
#include <glm/gtc/matrix_transform.hpp>
#include "RenderCommand.h"
#include "Engine/Profile/Instrumentor.h"

namespace Engine::Renderer
{
    Renderer2D::Scene2DData Renderer2D::m_SceneData = Renderer2D::Scene2DData();
    Renderer2D::Statistics Renderer2D::m_Stats;

    const glm::vec4 Renderer2D::QuadVertexPositions[4] =
        {
            {-0.5f, -0.5f, 0.0f, 1.0f},
            {0.5f, -0.5f, 0.0f, 1.0f},
            {0.5f, 0.5f, 0.0f, 1.0f},
            {-0.5f, 0.5f, 0.0f, 1.0f}};
    const glm::vec2 Renderer2D::QuadTexCoords[4] = {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f}};

    void Renderer2D::Init()
    {
        ENGINE_PROFILING_FUNC();

        // initialize shaders
        m_SceneData.QuadTextureShader = Shader::Create("assets/shaders/Renderer2D_QuadShader.glsl");
        m_SceneData.CircleShader = Shader::Create("assets/shaders/Renderer2D_CircleShader.glsl");
        m_SceneData.LineShader = Shader::Create("assets/shaders/Renderer2D_LineShader.glsl");

        // initialize vertex array
        //-- Init Quad --
        {

            m_SceneData.QuadVertexArray = VertexArray::Create();

            m_SceneData.QuadVertexBufferBase = new QuadVertex[m_SceneData.MaxVertices];
            m_SceneData.QuadVertexBufferPtr = m_SceneData.QuadVertexBufferBase;

            m_SceneData.QuadVertexBuffer = VertexBuffer::Create(sizeof(QuadVertex) * m_SceneData.MaxVertices);
            m_SceneData.QuadVertexBuffer->SetLayout({{ShaderDataType::Float3, "a_Position"},
                                                     {ShaderDataType::Float4, "a_Color"},
                                                     {ShaderDataType::Float2, "a_TexCoord"},
                                                     {ShaderDataType::Float, "a_TexIndex"},
                                                     {ShaderDataType::Float, "a_TilingFactor"},
                                                     {ShaderDataType::Int, "a_EntityID"}});
            m_SceneData.QuadVertexArray->AddVertexBuffer(m_SceneData.QuadVertexBuffer);

            uint32_t *quadIndices = new uint32_t[m_SceneData.MaxIndices];
            uint32_t offset = 0;
            for (size_t i = 0; i < m_SceneData.MaxIndices; i += 6)
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

        //-- Init Circle --
        {
            m_SceneData.CircleVertexArray = VertexArray::Create();

            m_SceneData.CircleVertexBufferBase = new CircleVertex[m_SceneData.MaxVertices];
            m_SceneData.CircleVertexBufferPtr = m_SceneData.CircleVertexBufferBase;

            m_SceneData.CircleVertexBuffer = VertexBuffer::Create(sizeof(CircleVertex) * m_SceneData.MaxVertices);
            m_SceneData.CircleVertexBuffer->SetLayout({{ShaderDataType::Float3, "a_WorldPosition"},
                                                       {ShaderDataType::Float2, "a_LocalPosition"},
                                                       {ShaderDataType::Float4, "a_Color"},
                                                       {ShaderDataType::Float, "a_Thickness"},
                                                       {ShaderDataType::Float, "a_Fade"},
                                                       {ShaderDataType::Int, "a_EntityID"}});
            m_SceneData.CircleVertexArray->AddVertexBuffer(m_SceneData.CircleVertexBuffer);

            uint32_t *circleIndices = new uint32_t[m_SceneData.MaxIndices];
            uint32_t offset = 0;
            for (size_t i = 0; i < m_SceneData.MaxIndices; i += 6)
            {
                circleIndices[i + 0] = offset + 0;
                circleIndices[i + 1] = offset + 1;
                circleIndices[i + 2] = offset + 2;

                circleIndices[i + 3] = offset + 2;
                circleIndices[i + 4] = offset + 3;
                circleIndices[i + 5] = offset + 0;

                offset += 4;
            }

            Ref<IndexBuffer> circleIB = IndexBuffer::Create(circleIndices, m_SceneData.MaxIndices);
            m_SceneData.CircleVertexArray->SetIndexBuffer(circleIB);
            delete[] circleIndices;
        }

        //-- Init Line --
        {
            m_SceneData.LineVertexArray = VertexArray::Create();

            m_SceneData.LineVertexBufferBase = new LineVertex[m_SceneData.MaxVertices];
            m_SceneData.LineVertexBufferPtr = m_SceneData.LineVertexBufferBase;

            m_SceneData.LineVertexBuffer = VertexBuffer::Create(sizeof(LineVertex) * m_SceneData.MaxVertices);
            m_SceneData.LineVertexBuffer->SetLayout({{ShaderDataType::Float3, "a_Position"},
                                                     {ShaderDataType::Float4, "a_Color"},
                                                     {ShaderDataType::Int, "a_EntityID"}});
            m_SceneData.LineVertexArray->AddVertexBuffer(m_SceneData.LineVertexBuffer);
            SetLineWidth(m_SceneData.LineWidth);
        }
        m_SceneData.CameraUniformBuffer = UniformBuffer::Create(sizeof(CameraData), 0);
    }

    void Renderer2D::Shutdown()
    {
        ENGINE_PROFILING_FUNC();
    }

    void Renderer2D::BeginScene(glm::mat4 viewProjectionMatrix)
    {
        ENGINE_PROFILING_FUNC();

        m_SceneData.CameraBuffer.ViewProjection = viewProjectionMatrix;
        m_SceneData.CameraUniformBuffer->SetData(&m_SceneData.CameraBuffer, sizeof(CameraData));

        ResetQuad();
        ResetCircle();
        ResetLine();
    }

    void Renderer2D::EndScene()
    {
        ENGINE_PROFILING_FUNC();

        // Quad
        {
            uint32_t dataSize = (uint32_t)((uint8_t *)m_SceneData.QuadVertexBufferPtr - (uint8_t *)m_SceneData.QuadVertexBufferBase);
            m_SceneData.QuadVertexBuffer->SetData(m_SceneData.QuadVertexBufferBase, dataSize);
            FlushQuad();
        }

        // Circle
        {
            uint32_t dataSize = (uint32_t)((uint8_t *)m_SceneData.CircleVertexBufferPtr - (uint8_t *)m_SceneData.CircleVertexBufferBase);
            m_SceneData.CircleVertexBuffer->SetData(m_SceneData.CircleVertexBufferBase, dataSize);
            FlushCircle();
        }

        // Line
        {
            uint32_t dataSize = (uint32_t)((uint8_t *)m_SceneData.LineVertexBufferPtr - (uint8_t *)m_SceneData.LineVertexBufferBase);
            m_SceneData.LineVertexBuffer->SetData(m_SceneData.LineVertexBufferBase, dataSize);
            FlushLine();
        }
    }

    /// @brief flush the draw calls
    void Renderer2D::FlushQuad()
    {
        ENGINE_PROFILING_FUNC();
        for (int i = 0; i < m_SceneData.TextureSlotIndex; i++)
        {
            m_SceneData.TextureSlots[i]->Bind(i);
        }
        m_Stats.DrawCalls++;
        m_SceneData.QuadTextureShader->Bind();
        RenderCommand::DrawIndexed(m_SceneData.QuadVertexArray, m_SceneData.QuadIndexCount);
    }

    void Renderer2D::FlushCircle()
    {
        ENGINE_PROFILING_FUNC();
        m_Stats.DrawCalls++;
        m_SceneData.CircleShader->Bind();
        RenderCommand::DrawIndexed(m_SceneData.CircleVertexArray, m_SceneData.CircleIndexCount);
    }

    void Renderer2D::FlushLine()
    {
        ENGINE_PROFILING_FUNC();
        m_Stats.DrawCalls++;
        m_SceneData.LineShader->Bind();
        RenderCommand::DrawLine(m_SceneData.LineVertexArray, m_SceneData.LineVertexCount);
    }

#pragma region DrawQuadImplementations
    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color, int entityID)
    {
        DrawQuad(glm::vec3(position, 0.0f), size, color, entityID);
    }

    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color, int entityID)
    {
        ENGINE_PROFILING_FUNC();

        const float TextureIndex = 0.0f; // White Texture
        const float TilingFactor = 1.0f;

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                              glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

        DrawQuadInternal(transform, color, TextureIndex, TilingFactor, entityID);
    }

    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, const float tintFactor, const glm::vec4 &tintColor, int entityID)
    {
        DrawQuad(glm::vec3(position, 0.0f), size, texture, tintFactor, tintColor, entityID);
    }

    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, const float tintFactor, const glm::vec4 &tintColor, int entityID)
    {
        ENGINE_PROFILING_FUNC();

        const float TextureIndex = GetTextureIndex(texture);

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                              glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

        DrawQuadInternal(transform, tintColor, TextureIndex, tintFactor, entityID);
    }

    void Renderer2D::DrawRotateQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color, const float z_degrees, int entityID)
    {
        DrawRotateQuad(glm::vec3(position, 0.0f), size, color, z_degrees, entityID);
    }

    void Renderer2D::DrawRotateQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color, const float z_degrees, int entityID)
    {
        ENGINE_PROFILING_FUNC();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(z_degrees), glm::vec3(0.0f, 0.0f, 1.0f)) *
                              glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

        const float TextureIndex = 0.0f; // White Texture
        const float TilingFactor = 1.0f;
        DrawQuadInternal(transform, color, TextureIndex, TilingFactor, entityID);
    }

    void Renderer2D::DrawRotateQuad(const glm::vec2 &position, const glm::vec2 &size, const float z_degrees, const Ref<Texture2D> &texture, const float tintFactor, const glm::vec4 &tintColor, int entityID)
    {
        DrawRotateQuad(glm::vec3(position, 0.0f), size, z_degrees, texture, tintFactor, tintColor, entityID);
    }

    void Renderer2D::DrawRotateQuad(const glm::vec3 &position, const glm::vec2 &size, const float z_degrees, const Ref<Texture2D> &texture, const float tintFactor, const glm::vec4 &tintColor, int entityID)
    {
        ENGINE_PROFILING_FUNC();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(z_degrees), glm::vec3(0.0f, 0.0f, 1.0f)) *
                              glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

        const float TextureIndex = GetTextureIndex(texture);
        const float TilingFactor = 1.0f;

        DrawQuadInternal(transform, tintColor, TextureIndex, TilingFactor, entityID);
    }

    void Renderer2D::DrawSubTextureQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<SubTexture2D> &texture, const float tintFactor, const glm::vec4 &tintColor, int entityID)
    {
        ENGINE_PROFILING_FUNC();
        DrawSubTextureQuad(glm::vec3(position, 0.0f), size, texture, tintFactor, tintColor, entityID);
    }

    void Renderer2D::DrawSubTextureQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<SubTexture2D> &subTexture, const float tintFactor, const glm::vec4 &tintColor, int entityID)
    {
        ENGINE_PROFILING_FUNC();

        const float TextureIndex = GetTextureIndex(subTexture->GetTexture());

        glm::vec2 spriteSize = subTexture->GetTexCoords()[2] - subTexture->GetTexCoords()[0];

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                              glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

        const float TilingFactor = 1.0f;

        DrawQuadInternal(transform, tintColor, TextureIndex, tintFactor, entityID, subTexture->GetTexCoords());
    }

    void Renderer2D::DrawRotateSubTextureQuad(const glm::vec2 &position, const glm::vec2 &size, const float z_degrees, const Ref<SubTexture2D> &subTexture, const float tintFactor, const glm::vec4 &tintColor, int entityID)
    {
        DrawRotateSubTextureQuad(glm::vec3(position, 0.0f), size, z_degrees, subTexture, tintFactor, tintColor, entityID);
    }

    void Renderer2D::DrawRotateSubTextureQuad(const glm::vec3 &position, const glm::vec2 &size, const float z_degrees, const Ref<SubTexture2D> &subTexture, const float tintFactor, const glm::vec4 &tintColor, int entityID)
    {
        ENGINE_PROFILING_FUNC();

        const float TextureIndex = GetTextureIndex(subTexture->GetTexture());

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(z_degrees), glm::vec3(0.0f, 0.0f, 1.0f)) *
                              glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

        const float TilingFactor = 1.0f;

        DrawQuadInternal(transform, tintColor, TextureIndex, tintFactor, entityID, subTexture->GetTexCoords());
    }

    void Renderer2D::DrawQuad(const glm::mat4 &transform, const glm::vec4 &color, int entityID)
    {
        ENGINE_PROFILING_FUNC();

        const float TextureIndex = 0.0f; // White Texture
        const float TilingFactor = 1.0f;

        DrawQuadInternal(transform, color, TextureIndex, TilingFactor, entityID);
    }

    void Renderer2D::DrawQuad(const glm::mat4 &transform, const Ref<Texture2D> &texture, const float tintFactor, const glm::vec4 &tintColor, int entityID)
    {
        ENGINE_PROFILING_FUNC();

        const float TextureIndex = GetTextureIndex(texture);
        const float TilingFactor = 1.0f;

        DrawQuadInternal(transform, tintColor, TextureIndex, TilingFactor, entityID);
    }

    void Renderer2D::DrawCircle(const glm::vec3 &position, const float radius, const glm::vec4 &color, float thickness, float fade, int entityID)
    {
        ENGINE_PROFILING_FUNC();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                              glm::scale(glm::mat4(1.0f), glm::vec3(radius * 2.0f, radius * 2.0f, 1.0f));

        DrawCircleInternal(transform, color, thickness, fade, entityID);
    }

    void Renderer2D::DrawLine(const glm::vec2 &p0, const glm::vec2 &p1, const glm::vec4 &color, int entityID)
    {
        DrawLine(glm::vec3(p0, 0.0f), glm::vec3(p1, 0.0f), color, entityID);
    }

    void Renderer2D::DrawLine(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec4 &color, int entityID)
    {
        ENGINE_PROFILING_FUNC();

        if (m_SceneData.LineVertexCount >= m_SceneData.MaxVertices)
        {
            UploadLineData();
            FlushLine();
            ResetLine();
        }

        m_SceneData.LineVertexBufferPtr->Position = p0;
        m_SceneData.LineVertexBufferPtr->Color = color;
        m_SceneData.LineVertexBufferPtr->EntityID = entityID;
        m_SceneData.LineVertexBufferPtr++;

        m_SceneData.LineVertexBufferPtr->Position = p1;
        m_SceneData.LineVertexBufferPtr->Color = color;
        m_SceneData.LineVertexBufferPtr->EntityID = entityID;
        m_SceneData.LineVertexBufferPtr++;

        m_SceneData.LineVertexCount += 2;

        ENGINE_ASSERT(m_SceneData.LineVertexCount <= m_SceneData.MaxVertices, "Renderer2D::DrawLine - Exceeded max line vertex count!");
    }

    void Renderer2D::SetLineWidth(float width)
    {
        m_SceneData.LineWidth = width;
        RenderCommand::SetLineWidth(width);
    }

    float Renderer2D::GetLineWidth()
    {
        return m_SceneData.LineWidth;
    }

    void Renderer2D::DrawRect(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color, int entityID)
    {
        DrawRect(glm::vec3(position, 0.0f), size, color, entityID);
    }

    void Renderer2D::DrawRect(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color, int entityID)
    {
        glm::vec3 p0 = position + glm::vec3(-size.x * 0.5f, -size.y * 0.5f, 0.0f);
        glm::vec3 p1 = position + glm::vec3(size.x * 0.5f, -size.y * 0.5f, 0.0f);
        glm::vec3 p2 = position + glm::vec3(size.x * 0.5f, size.y * 0.5f, 0.0f);
        glm::vec3 p3 = position + glm::vec3(-size.x * 0.5f, size.y * 0.5f, 0.0f);

        DrawLine(p0, p1, color, entityID);
        DrawLine(p1, p2, color, entityID);
        DrawLine(p2, p3, color, entityID);
        DrawLine(p3, p0, color, entityID);
    }

    void Renderer2D::DrawRect(const glm::mat4 &transform, const glm::vec4 &color, int entityID)
    {
        glm::vec3 p0 = transform * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
        glm::vec3 p1 = transform * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
        glm::vec3 p2 = transform * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
        glm::vec3 p3 = transform * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);

        DrawLine(p0, p1, color, entityID);
        DrawLine(p1, p2, color, entityID);
        DrawLine(p2, p3, color, entityID);
        DrawLine(p3, p0, color, entityID);
    }

    void Renderer2D::DrawSprite(const glm::mat4 &transform, SpriteComponent &src, int entityID)
    {
        ENGINE_PROFILING_FUNC();
        auto color = src.Color;
        float TilingFactor = src.TilingFactor;
        if (src.Texture)
        {
            float TextureIndex = GetTextureIndex(src.Texture);
            DrawQuadInternal(transform, color, TextureIndex, TilingFactor, entityID);
            return;
        }
        else
        {
            float TextureIndex = 0.0f; // White Texture
            DrawQuadInternal(transform, color, TextureIndex, TilingFactor, entityID);
        }
    }

#pragma endregion

#pragma region DrawCircleImplementations
    void Renderer2D::DrawCircle(const glm::mat4 &transform, CircleComponent &src, int entityID)
    {
        ENGINE_PROFILING_FUNC();
        DrawCircleInternal(transform, src.Color, src.Thickness, src.Fade, entityID);
    }

    void Renderer2D::DrawCircle(const glm::mat4 &transform, glm::vec4 color, float thickness, float fade, int entityID)
    {
        ENGINE_PROFILING_FUNC();
        DrawCircleInternal(transform, color, thickness, fade, entityID);
    }

#pragma endregion

    /// @brief get the texture index in slot
    /// if not exist, add it to slot
    /// if exceed max slots, flush and reset
    float Renderer2D::GetTextureIndex(const Ref<Texture2D> &texture)
    {
        for (uint32_t i = 1; i < m_SceneData.TextureSlotIndex; i++)
        {
            if (Texture::IsEqual(*m_SceneData.TextureSlots[i], *texture))
            {
                // ENGINE_INFO("Existing Texture Slot: {}", i);
                return (float)i;
            }
        }

        // New Texture
        if (m_SceneData.TextureSlotIndex >= Renderer2D::Scene2DData::MaxTextureSlots)
        {
            UploadQuadData();
            FlushQuad();
            ResetQuad();
        }

        // ENGINE_INFO("New Texture Slot: {}", m_SceneData.TextureSlotIndex);

        m_SceneData.TextureSlots[m_SceneData.TextureSlotIndex] = texture;
        return (float)(m_SceneData.TextureSlotIndex++);
    }
    /// @brief upload the quad vertex data to gpu
    void Renderer2D::UploadQuadData()
    {
        uint32_t dataSize = (uint32_t)((uint8_t *)m_SceneData.QuadVertexBufferPtr - (uint8_t *)m_SceneData.QuadVertexBufferBase);
        m_SceneData.QuadVertexBuffer->SetData(m_SceneData.QuadVertexBufferBase, dataSize);
    }
    /// @brief upload the circle vertex data to gpu
    void Renderer2D::UploadCircleData()
    {
        uint32_t dataSize = (uint32_t)((uint8_t *)m_SceneData.CircleVertexBufferPtr - (uint8_t *)m_SceneData.CircleVertexBufferBase);
        m_SceneData.CircleVertexBuffer->SetData(m_SceneData.CircleVertexBufferBase, dataSize);
    }
    /// @brief upload the line vertex data to gpu
    void Renderer2D::UploadLineData()
    {
        uint32_t dataSize = (uint32_t)((uint8_t *)m_SceneData.LineVertexBufferPtr - (uint8_t *)m_SceneData.LineVertexBufferBase);
        m_SceneData.LineVertexBuffer->SetData(m_SceneData.LineVertexBufferBase, dataSize);
    }

    void Renderer2D::ResetQuad()
    {
        m_SceneData.QuadVertexBufferPtr = m_SceneData.QuadVertexBufferBase;
        m_SceneData.QuadIndexCount = 0;
        m_SceneData.TextureSlotIndex = 1;
    }
    void Renderer2D::ResetCircle()
    {
        m_SceneData.CircleVertexBufferPtr = m_SceneData.CircleVertexBufferBase;
        m_SceneData.CircleIndexCount = 0;
    }
    void Renderer2D::ResetLine()
    {
        m_SceneData.LineVertexBufferPtr = m_SceneData.LineVertexBufferBase;
        m_SceneData.LineVertexCount = 0;
    }

    /// @brief internal function to draw quad
    void Renderer2D::DrawQuadInternal(const glm::mat4 &transform, const glm::vec4 &color, const float textureIndex, const float tilingFactor, int entityID, const glm::vec2 *texCoords)
    {
        if (m_SceneData.QuadIndexCount >= m_SceneData.MaxIndices)
        {
            UploadQuadData();
            FlushQuad();
            ResetQuad();
        }

        for (int i = 0; i < 4; i++)
        {
            m_SceneData.QuadVertexBufferPtr->Position = transform * QuadVertexPositions[i];
            m_SceneData.QuadVertexBufferPtr->Color = color;
            m_SceneData.QuadVertexBufferPtr->TexCoord = texCoords[i];
            m_SceneData.QuadVertexBufferPtr->TexIndex = textureIndex;
            m_SceneData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
            m_SceneData.QuadVertexBufferPtr->EntityID = entityID;
            m_SceneData.QuadVertexBufferPtr++;
        }

        m_SceneData.QuadIndexCount += 6;
        m_Stats.QuadCount++;

        ENGINE_ASSERT(m_SceneData.QuadIndexCount <= m_SceneData.MaxIndices, "Renderer2D::DrawQuad - Exceeded max quad index count!");
    }
    /// @brief internal function to draw circle
    void Renderer2D::DrawCircleInternal(const glm::mat4 &transform, const glm::vec4 &color, float thickness, float fade, int entityID)
    {
        if (m_SceneData.CircleIndexCount >= m_SceneData.MaxIndices)
        {
            UploadCircleData();
            FlushCircle();
            ResetCircle();
        }

        for (int i = 0; i < 4; i++)
        {
            m_SceneData.CircleVertexBufferPtr->WorldPosition = transform * QuadVertexPositions[i];
            m_SceneData.CircleVertexBufferPtr->LocalPosition = {QuadVertexPositions[i].x * 2.0f, QuadVertexPositions[i].y * 2.0f};
            m_SceneData.CircleVertexBufferPtr->Color = color;
            m_SceneData.CircleVertexBufferPtr->Thickness = thickness;
            m_SceneData.CircleVertexBufferPtr->Fade = fade;
            m_SceneData.CircleVertexBufferPtr->EntityID = entityID;
            m_SceneData.CircleVertexBufferPtr++;
        }

        m_SceneData.CircleIndexCount += 6;
        m_Stats.QuadCount++;

        ENGINE_ASSERT(m_SceneData.CircleIndexCount <= m_SceneData.MaxIndices, "Renderer2D::DrawCircle - Exceeded max circle index count!");
    }
}