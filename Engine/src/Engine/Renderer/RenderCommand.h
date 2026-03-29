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
#ifdef ENGINE_ENABLE_RENDERTHREAD
                        Submit([]()
                               { CurrentRendererAPI->Init(); });
#else
                        CurrentRendererAPI->Init();
#endif
                }

                inline static void SetClearColor(const glm::vec4 &color)
                {
#ifdef ENGINE_ENABLE_RENDERTHREAD
                        Submit([color]()
                               { CurrentRendererAPI->SetClearColor(color); });
#else
                        CurrentRendererAPI->SetClearColor(color);
#endif
                }

                inline static void Clear()
                {
#ifdef ENGINE_ENABLE_RENDERTHREAD
                        Submit([]()
                               { CurrentRendererAPI->Clear(); });
#else
                        CurrentRendererAPI->Clear();
#endif
                }

                inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
                {
#ifdef ENGINE_ENABLE_RENDERTHREAD
                        Submit([x, y, width, height]()
                               { CurrentRendererAPI->SetViewport(x, y, width, height); });
#else
                        CurrentRendererAPI->SetViewport(x, y, width, height);
#endif
                }

                inline static void DrawIndexed(Ref<VertexArray> &vertexArray, uint32_t indexCount = -1)
                {
#ifdef ENGINE_ENABLE_RENDERTHREAD
                        Submit([&vertexArray, indexCount]()
                               { CurrentRendererAPI->DrawIndexed(vertexArray, indexCount); });
#else
                        CurrentRendererAPI->DrawIndexed(vertexArray, indexCount);
#endif
                }

                inline static void DrawLine(Ref<VertexArray> &vertexArray, uint32_t vertexCount)
                {
#ifdef ENGINE_ENABLE_RENDERTHREAD
                        Submit([&vertexArray, vertexCount]()
                               { CurrentRendererAPI->DrawLine(vertexArray, vertexCount); });
#else
                        CurrentRendererAPI->DrawLine(vertexArray, vertexCount);
#endif
                }

                inline static void SetLineWidth(float width)
                {
#ifdef ENGINE_ENABLE_RENDERTHREAD
                        Submit([width]()
                               { CurrentRendererAPI->SetLineWidth(width); });
#else
                        CurrentRendererAPI->SetLineWidth(width);
#endif
                }

#ifdef ENGINE_ENABLE_RENDERTHREAD
        private:
                inline static void Submit(std::function<void()> &&renderCmd)
                {
                        Engine::Core::Application::Get().SubmitRendererCommand(std::move(renderCmd));
                }
#endif
        private:
                static RendererAPI *CurrentRendererAPI;
        };

} // namespace Engine::Renderer