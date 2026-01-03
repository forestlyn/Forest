#pragma once
#include "Engine/Renderer/RendererAPI.h"

namespace Engine::Platform::OpenGL
{
    class OpenGLRendererAPI : public Renderer::RendererAPI
    {
    public:
        OpenGLRendererAPI() = default;

        virtual void Init() override;
        virtual void SetClearColor(const glm::vec4 &color) override;
        virtual void Clear() override;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

        virtual void DrawIndexed(Ref<Renderer::VertexArray> &vertexArray, uint32_t indexCount = -1) override;
    };
} // namespace Engine::Platform::OpenGL