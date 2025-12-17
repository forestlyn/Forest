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

        inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
        {
            CurrentRendererAPI->SetViewport(x, y, width, height);
        }

        inline static void DrawIndexed(Ref<VertexArray> &vertexArray, uint32_t indexCount = 0)
        {
            CurrentRendererAPI->DrawIndexed(vertexArray, indexCount);
        }

    private:
        static RendererAPI *CurrentRendererAPI;
    };

} // namespace Engine::Renderer