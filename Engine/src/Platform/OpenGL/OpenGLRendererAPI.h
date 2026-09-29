#pragma once
#include "Engine/Renderer/RendererAPI.h"
#include <vector>

namespace Engine::Platform::OpenGL
{
    class OpenGLRendererAPI : public Renderer::RendererAPI
    {
    public:
        OpenGLRendererAPI() = default;

        virtual void Init() override;
        virtual void SetClearColor(const glm::vec4 &color) override;
        virtual void Clear() override;
        void BeginOverlay() override;
        void EndOverlay() override;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

        virtual void DrawIndexed(const Ref<Renderer::VertexArray> &vertexArray, uint32_t indexCount = -1) override;
        virtual void DrawLine(const Ref<Renderer::VertexArray> &vertexArray, uint32_t vertexCount) override;
        virtual void SetLineWidth(float width) override;
    private:
        struct OverlayState
        {
            bool DepthTest, DepthWrite, Blend, Cull, Scissor;
            int SrcRGB, DstRGB, SrcAlpha, DstAlpha, EquationRGB, EquationAlpha;
        };
        std::vector<OverlayState> m_OverlayStates;
    };
} // namespace Engine::Platform::OpenGL
