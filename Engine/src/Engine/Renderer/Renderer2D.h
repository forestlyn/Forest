#pragma once
#include "Engine/pcheader.h"
#include <glm/glm.hpp>
#include "Engine/Renderer/Camera/Camera.h"
#include "Engine/Renderer/Shader/Shader.h"
#include "Engine/Renderer/Shader/Texture.h"
#include "Engine/Renderer/Shader/SubTexture.h"
#include "Engine/Renderer/Shader/VertexArray.h"
namespace Engine::Renderer
{

    class Renderer2D
    {
    public:
        struct Statistics
        {
            uint32_t DrawCalls = 0;
            uint32_t QuadCount = 0;

            uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
            uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
        };

    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(const Camera &camera);
        static void EndScene();

        static void Flush();

        static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color);
        static void DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color);
        static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, const float tintFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));
        static void DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, const float tintFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));

        static void DrawRotateQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color, const float z_degrees);
        static void DrawRotateQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color, const float z_degrees);
        static void DrawRotateQuad(const glm::vec2 &position, const glm::vec2 &size, const float z_degrees, const Ref<Texture2D> &texture, const float tintFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));
        static void DrawRotateQuad(const glm::vec3 &position, const glm::vec2 &size, const float z_degrees, const Ref<Texture2D> &texture, const float tintFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));

        static void DrawSubTextureQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<SubTexture2D> &subTexture, const float tintFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));
        static void DrawSubTextureQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<SubTexture2D> &subTexture, const float tintFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));
        static void DrawRotateSubTextureQuad(const glm::vec2 &position, const glm::vec2 &size, const float z_degrees, const Ref<SubTexture2D> &subTexture, const float tintFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));
        static void DrawRotateSubTextureQuad(const glm::vec3 &position, const glm::vec2 &size, const float z_degrees, const Ref<SubTexture2D> &subTexture, const float tintFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));

        static void DrawQuad(const glm::mat4 &transform, const glm::vec4 &color);
        static void DrawQuad(const glm::mat4 &transform, const Ref<Texture2D> &texture, const float tintFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));

        // just for test
        static int GetMaxQuads()
        {
            return m_SceneData.MaxQuads;
        }
        static void SetMaxQuads(uint32_t maxQuads)
        {
            if (maxQuads > m_SceneData.InitialMaxQuads)
            {
                ENGINE_WARN("Setting max quads beyond initial max quads will cause memory issues!");
                return;
            }
            m_SceneData.MaxQuads = maxQuads;
            m_SceneData.MaxVertices = m_SceneData.MaxQuads * 4;
            m_SceneData.MaxIndices = m_SceneData.MaxQuads * 6;
        }

        static Statistics GetStats()
        {
            return m_Stats;
        }

        static void ResetStats()
        {
            m_Stats.DrawCalls = 0;
            m_Stats.QuadCount = 0;
        }

    private:
        const static glm::vec4 QuadVertexPositions[4];
        const static glm::vec2 QuadTexCoords[4];

        static float GetTextureIndex(const Ref<Texture2D> &texture);

        static void UploadQuadData();
        static void Reset();

        static void DrawQuadInternal(const glm::mat4 &transform, const glm::vec4 &color, const float textureIndex, const float tilingFactor, const glm::vec2 *texCoords = QuadTexCoords);

        struct QuadVertex
        {
            glm::vec3 Position;
            glm::vec4 Color;
            glm::vec2 TexCoord;
            float TexIndex;
            float TilingFactor;
        };

        struct Scene2DData
        {
            //-- Camera data --
            glm::mat4 ViewProjectionMatrix;

            //-- Textures and shaders --
            Ref<Shader> QuadTextureShader;

            static const int MaxTextureSlots = 32;
            std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
            uint32_t TextureSlotIndex = 1; // 0 = white texture

            //-- Quad data --
            const int InitialMaxQuads = 10000;

            uint32_t MaxQuads = InitialMaxQuads;
            uint32_t MaxVertices = MaxQuads * 4;
            uint32_t MaxIndices = MaxQuads * 6;

            Ref<VertexArray> QuadVertexArray;
            Ref<VertexBuffer> QuadVertexBuffer;
            uint32_t QuadIndexCount = 0;

            QuadVertex *QuadVertexBufferBase = nullptr;
            QuadVertex *QuadVertexBufferPtr = nullptr;
        };

        static Scene2DData m_SceneData;
        static Statistics m_Stats;
    };

} // namespace Engine::Renderer