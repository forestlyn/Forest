#pragma once

#include "RendererAPI.h"
namespace Engine::Renderer
{

    class RenderCommand
    {
    public:
        static void SetClearColor(const glm::vec4 &color)
        {
            CurrentRendererAPI->SetClearColor(color);
        }

        static void Clear()
        {
            CurrentRendererAPI->Clear();
        }

        static void Submit(std::shared_ptr<VertexArray> &vertexArray)
        {
            CurrentRendererAPI->Submit(vertexArray);
        }

    private:
        static RendererAPI *CurrentRendererAPI;
    };

} // namespace Engine::Renderer