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

        static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color);
        static void DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color);
        static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<Texture2D> &texture);
        static void DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture);
        static void DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const float z_rotation,
                             const Ref<Texture2D> &texture, const glm::vec4 &tintColor);

    private:
        struct Scene2DData
        {
            glm::mat4 ViewProjectionMatrix;

            Ref<Shader> QuadTextureShader;
            Ref<VertexArray> QuadVertexArray;
            Ref<Texture2D> WhiteTexture;
        };

        static Scene2DData *m_SceneData;
    };

} // namespace Engine::Renderer