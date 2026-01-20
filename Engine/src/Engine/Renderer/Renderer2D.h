#pragma once
#include "Engine/pcheader.h"
#include <glm/glm.hpp>
#include "Engine/Renderer/Shader/Shader.h"
#include "Engine/Renderer/Shader/Texture.h"
#include "Engine/Renderer/Shader/SubTexture.h"
#include "Engine/Renderer/Shader/VertexArray.h"
#include "Engine/Scene/Components/SpriteComponent.h"
#include "Engine/Scene/Components/CircleComponent.h"
#include "Engine/Renderer/UniformBuffer.h"

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

        static void BeginScene(glm::mat4 viewProjectionMatrix);
        static void EndScene();

        static void FlushQuad();
        static void FlushCircle();
        static void FlushLine();

        static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color, int entityID = -1);
        static void DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color, int entityID = -1);
        static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, const float tintFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f), int entityID = -1);
        static void DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, const float tintFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f), int entityID = -1);

        static void DrawRotateQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color, const float z_degrees, int entityID = -1);
        static void DrawRotateQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color, const float z_degrees, int entityID = -1);
        static void DrawRotateQuad(const glm::vec2 &position, const glm::vec2 &size, const float z_degrees, const Ref<Texture2D> &texture, const float tintFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f), int entityID = -1);
        static void DrawRotateQuad(const glm::vec3 &position, const glm::vec2 &size, const float z_degrees, const Ref<Texture2D> &texture, const float tintFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f), int entityID = -1);

        static void DrawSubTextureQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<SubTexture2D> &subTexture, const float tintFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f), int entityID = -1);
        static void DrawSubTextureQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<SubTexture2D> &subTexture, const float tintFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f), int entityID = -1);
        static void DrawRotateSubTextureQuad(const glm::vec2 &position, const glm::vec2 &size, const float z_degrees, const Ref<SubTexture2D> &subTexture, const float tintFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f), int entityID = -1);
        static void DrawRotateSubTextureQuad(const glm::vec3 &position, const glm::vec2 &size, const float z_degrees, const Ref<SubTexture2D> &subTexture, const float tintFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f), int entityID = -1);

        static void DrawQuad(const glm::mat4 &transform, const glm::vec4 &color, int entityID = -1);
        static void DrawQuad(const glm::mat4 &transform, const Ref<Texture2D> &texture, const float tintFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f), int entityID = -1);

        static void DrawCircle(const glm::vec3 &position, const float radius, const glm::vec4 &color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);

        static void DrawLine(const glm::vec2 &p0, const glm::vec2 &p1, const glm::vec4 &color, int entityID = -1);
        static void DrawLine(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec4 &color, int entityID = -1);
        static void SetLineWidth(float width);
        static float GetLineWidth();

        static void DrawRect(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color, int entityID = -1);
        static void DrawRect(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color, int entityID = -1);
        static void DrawRect(const glm::mat4 &transform, const glm::vec4 &color, int entityID = -1);
        // draw component
        static void DrawSprite(const glm::mat4 &transform, SpriteComponent &src, int entityID);
        static void DrawCircle(const glm::mat4 &transform, CircleComponent &src, int entityID);
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
        static void ResetQuad();

        static void UploadCircleData();
        static void ResetCircle();

        static void UploadLineData();
        static void ResetLine();

        static void DrawQuadInternal(const glm::mat4 &transform, const glm::vec4 &color, const float textureIndex, const float tilingFactor, int entityID = -1, const glm::vec2 *texCoords = QuadTexCoords);
        static void DrawCircleInternal(const glm::mat4 &transform, const glm::vec4 &color, float thickness, float fade, int entityID = -1);
        struct QuadVertex
        {
            glm::vec3 Position;
            glm::vec4 Color;
            glm::vec2 TexCoord;
            float TexIndex;
            float TilingFactor;
            int EntityID;
        };
        struct CircleVertex
        {
            glm::vec3 WorldPosition;
            glm::vec2 LocalPosition;
            glm::vec4 Color;
            float Thickness;
            float Fade;
            int EntityID;
        };
        struct LineVertex
        {
            glm::vec3 Position;
            glm::vec4 Color;
            int EntityID;
        };
        struct CameraData
        {
            glm::mat4 ViewProjection;
        };
        struct Scene2DData
        {
            //-- Camera data --
            glm::mat4 ViewProjectionMatrix;

            //-- Textures and shaders --
            Ref<Shader> QuadTextureShader;
            Ref<Shader> CircleShader;
            Ref<Shader> LineShader;

            static const int MaxTextureSlots = 32;
            std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
            uint32_t TextureSlotIndex = 1; // 0 = white texture

            const int InitialMaxQuads = 10000;

            uint32_t MaxQuads = InitialMaxQuads;
            uint32_t MaxVertices = MaxQuads * 4;
            uint32_t MaxIndices = MaxQuads * 6;

            //-- Quad data --

            Ref<VertexArray> QuadVertexArray;
            Ref<VertexBuffer> QuadVertexBuffer;
            uint32_t QuadIndexCount = 0;

            QuadVertex *QuadVertexBufferBase = nullptr;
            QuadVertex *QuadVertexBufferPtr = nullptr;

            // -- Circle data --
            Ref<VertexArray> CircleVertexArray;
            Ref<VertexBuffer> CircleVertexBuffer;
            uint32_t CircleIndexCount = 0;
            CircleVertex *CircleVertexBufferBase = nullptr;
            CircleVertex *CircleVertexBufferPtr = nullptr;

            // -- Line data --
            Ref<VertexArray> LineVertexArray;
            Ref<VertexBuffer> LineVertexBuffer;
            uint32_t LineVertexCount = 0;
            LineVertex *LineVertexBufferBase = nullptr;
            LineVertex *LineVertexBufferPtr = nullptr;
            float LineWidth = 2.0f;

            CameraData CameraBuffer;
            Ref<UniformBuffer> CameraUniformBuffer;
        };

        static Scene2DData m_SceneData;
        static Statistics m_Stats;
    };

} // namespace Engine::Renderer