#pragma once
#include "RendererAPI.h"
#include "Shader.h"
#include "Camera/Camera.h"
namespace Engine::Renderer
{

    class Renderer
    {
    public:
        static void Init();
        static void BeginScene(const Camera &camera);
        static void EndScene();

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