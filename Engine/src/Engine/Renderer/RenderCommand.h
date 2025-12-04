#pragma once

#include "RendererAPI.h"
namespace Engine::Renderer
{

    class RenderCommand
    {
    public:
        inline static void Init()
        {
            CurrentRendererAPI->Init();
        }

        inline static void SetClearColor(const glm::vec4 &color)
        {
            CurrentRendererAPI->SetClearColor(color);
        }

        inline static void Clear()
        {
            CurrentRendererAPI->Clear();
        }

        inline static void DrawIndexed(Ref<VertexArray> &vertexArray)
        {
            CurrentRendererAPI->DrawIndexed(vertexArray);
        }

    private:
        static RendererAPI *CurrentRendererAPI;
    };

} // namespace Engine::Renderer