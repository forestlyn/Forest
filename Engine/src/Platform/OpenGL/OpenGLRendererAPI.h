#pragma once
#include "Engine/Renderer/RendererAPI.h"

namespace Engine::Platform::OpenGL
{
    class OpenGLRendererAPI : public Renderer::RendererAPI
    {
    public:
        OpenGLRendererAPI() = default;

        virtual void SetClearColor(const glm::vec4 &color) override;
        virtual void Clear() override;

        virtual void DrawIndexed(std::shared_ptr<Renderer::VertexArray> &vertexArray) override;
    };
} // namespace Engine::Platform::OpenGL