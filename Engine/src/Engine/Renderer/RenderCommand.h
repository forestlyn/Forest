#pragma once

#include "RendererAPI.h"
#include "Engine/Core/Application.h"
namespace Engine::Renderer
{
    class RenderCommand
    {
    public:
        inline static void Init()
        {
            ENQUEUE_RENDER_COMMAND()
            CurrentRendererAPI->Init();
            ENQUEUE_RENDER_COMMAND_END()
        }
        inline static void SetClearColor(const glm::vec4 &color)
        {
            ENQUEUE_RENDER_COMMAND(color)
            CurrentRendererAPI->SetClearColor(color);
            ENQUEUE_RENDER_COMMAND_END()
        }
        inline static void Clear()
        {
            ENQUEUE_RENDER_COMMAND()
            CurrentRendererAPI->Clear();
            ENQUEUE_RENDER_COMMAND_END()
        }
        inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
        {
            ENQUEUE_RENDER_COMMAND(x, y, width, height)
            CurrentRendererAPI->SetViewport(x, y, width, height);
            ENQUEUE_RENDER_COMMAND_END()
        }
        inline static void DrawIndexed(Ref<VertexArray> &vertexArray, uint32_t indexCount = -1)
        {
            ENQUEUE_RENDER_COMMAND(vertexArray, indexCount)
            CurrentRendererAPI->DrawIndexed(vertexArray, indexCount);
            ENQUEUE_RENDER_COMMAND_END()
        }
        inline static void DrawLine(Ref<VertexArray> &vertexArray, uint32_t vertexCount)
        {
            ENQUEUE_RENDER_COMMAND(vertexArray, vertexCount)
            CurrentRendererAPI->DrawLine(vertexArray, vertexCount);
            ENQUEUE_RENDER_COMMAND_END()
        }
        inline static void SetLineWidth(float width)
        {
            ENQUEUE_RENDER_COMMAND(width)
            CurrentRendererAPI->SetLineWidth(width);
            ENQUEUE_RENDER_COMMAND_END()
        }

    private:
        static RendererAPI *CurrentRendererAPI;
    };
} // namespace Engine::Renderer