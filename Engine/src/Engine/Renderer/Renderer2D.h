#pragma once
#include "Engine/pcheader.h"
#include <glm/glm.hpp>
#include "Engine/Renderer/Camera/Camera.h"
#include "Engine/Renderer/Shader/Shader.h"
#include "Engine/Renderer/Shader/Texture.h"
#include "Engine/Renderer/Shader/VertexArray.h"
namespace Engine::Renderer
{

    class Renderer2D
    {
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

        static void DrawRotateQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color, const float z_rotation);
        static void DrawRotateQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color, const float z_rotation);
        static void DrawRotateQuad(const glm::vec2 &position, const glm::vec2 &size, const float z_rotation, const Ref<Texture2D> &texture, const float tintFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));
        static void DrawRotateQuad(const glm::vec3 &position, const glm::vec2 &size, const float z_rotation, const Ref<Texture2D> &texture, const float tintFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));

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

    private:
        struct QuadVertex
        {
            glm::vec3 Position;
            glm::vec4 Color;
            glm::vec2 TexCoord;
            // texture
        };

        struct Scene2DData
        {
            glm::mat4 ViewProjectionMatrix;

            Ref<VertexArray> QuadVertexArray;
            Ref<VertexBuffer> QuadVertexBuffer;

            Ref<Shader> QuadTextureShader;
            Ref<Texture2D> WhiteTexture;

            const int InitialMaxQuads = 10000;

            uint32_t MaxQuads = InitialMaxQuads;
            uint32_t MaxVertices = MaxQuads * 4;
            uint32_t MaxIndices = MaxQuads * 6;

            uint32_t QuadIndexCount = 0;

            QuadVertex *QuadVertexBufferBase = nullptr;
            QuadVertex *QuadVertexBufferPtr = nullptr;
        };

        static Scene2DData m_SceneData;
    };

} // namespace Engine::Renderer