#pragma once
#include "Engine/pcheader.h"
#include <glm/glm.hpp>
#include "Engine/Renderer/Camera/Camera.h"
#include "Engine/Renderer/Shader/Shader.h"
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

    private:
        struct Scene2DData
        {
            glm::mat4 ViewProjectionMatrix;

            Ref<Shader> QuadShader;
            Ref<VertexArray> QuadVertexArray;
        };

        static Scene2DData *m_SceneData;
    };

} // namespace Engine::Renderer