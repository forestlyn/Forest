#pragma once

#include "Engine/Renderer/VertexArray.h"
struct D3D12_INPUT_ELEMENT_DESC;
namespace Platform::DirectX12
{
    class DirectX12VertexArray : public Engine::Renderer::VertexArray
    {
    public:
        DirectX12VertexArray();
        virtual ~DirectX12VertexArray();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void AddVertexBuffer(const std::shared_ptr<Engine::Renderer::VertexBuffer> &vertexBuffer) override;
        virtual void SetIndexBuffer(const std::shared_ptr<Engine::Renderer::IndexBuffer> &indexBuffer) override;

        virtual const std::vector<std::shared_ptr<Engine::Renderer::VertexBuffer>> &GetVertexBuffers() const override
        {
            return m_VertexBuffers;
        }
        virtual const std::shared_ptr<Engine::Renderer::IndexBuffer> &GetIndexBuffer() const override
        {
            return m_IndexBuffer;
        }

    private:
        std::vector<std::shared_ptr<Engine::Renderer::VertexBuffer>> m_VertexBuffers;
        std::shared_ptr<Engine::Renderer::IndexBuffer> m_IndexBuffer;

        std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;
    };
} // namespace Platform::DirectX12
