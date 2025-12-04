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

        static void DrawIndexed(Ref<VertexArray> &vertexArray)
        {
            CurrentRendererAPI->DrawIndexed(vertexArray);
        }

    private:
        static RendererAPI *CurrentRendererAPI;
    };

} // namespace Engine::Renderer