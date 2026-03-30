#pragma once
#include "RendererAPI.h"
#include "Shader/Shader.h"
#include "Camera/Camera.h"
namespace Engine::Renderer
{

    class Renderer
    {
    public:
        static void Init();
        static void Shutdown();
        static void BeginScene(glm::mat4 viewProjectionMatrix);
        static void EndScene();

        static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

        static void Submit(Ref<Shader> &shader, Ref<VertexArray> &vertexArray, glm::mat4 transform = glm::mat4(1.0f));

        static inline RendererAPI::API GetAPI()
        {
            return RendererAPI::GetAPI();
        }

    private:
        struct SceneData
        {
            glm::mat4 ViewProjectionMatrix;
        };

        static SceneData *m_SceneData;
    };
} // namespace Engine::Renderer