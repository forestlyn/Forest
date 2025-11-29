#pragma once
#include "RendererAPI.h"
namespace Engine::Renderer
{

    class Renderer
    {
    public:
        static void BeginScene();
        static void EndScene();

        static void Submit(std::shared_ptr<VertexArray> &vertexArray);

        static inline RendererAPI::API GetAPI()
        {
            return RendererAPI::GetAPI();
        }
    };
} // namespace Engine::Renderer