#pragma once
#include "RendererAPI.h"
#include "Shader.h"
#include "Camera/Camera.h"
namespace Engine::Renderer
{

    class Renderer
    {
    public:
        static void BeginScene(const Camera &camera);
        static void EndScene();

        static void Submit(std::shared_ptr<Shader> &shader, std::shared_ptr<VertexArray> &vertexArray, glm::mat4 transform = glm::mat4(1.0f));

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