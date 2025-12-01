#pragma once
#include "Engine/Renderer/RendererAPI.h"

namespace Platform::DirectX12
{
    class DirectX12RendererAPI : public Engine::Renderer::RendererAPI
    {
    public:
        DirectX12RendererAPI() = default;

        virtual void SetClearColor(const glm::vec4 &color) override;
        virtual void Clear() override;

        virtual void DrawIndexed(std::shared_ptr<Engine::Renderer::VertexArray> &vertexArray) override;

    private:
        glm::vec4 m_ClearColor;
    };
} // namespace Platform::DirectX12