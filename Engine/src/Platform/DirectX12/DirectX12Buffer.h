#pragma once
#include "Engine/Renderer/Buffer.h"
#include <directx/d3d12.h>
// struct ID3D12Resource;
// struct D3D12_VERTEX_BUFFER_VIEW;

namespace Platform::DirectX12
{
    class DirectX12VertexBuffer : public Engine::Renderer::VertexBuffer
    {
    public:
        DirectX12VertexBuffer(float *vertices, uint32_t size);
        virtual ~DirectX12VertexBuffer();

        virtual void Bind() const override;
        virtual void Unbind() const override;
        virtual const Engine::Renderer::BufferLayout &GetLayout() const override;
        virtual void SetLayout(const Engine::Renderer::BufferLayout &layout) override;

    private:
        Engine::Renderer::BufferLayout m_Layout;
        // DirectX 12 specific members would go here
        ID3D12Resource *m_VertexBuffer;
        D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
    };

    class DirectX12IndexBuffer : public Engine::Renderer::IndexBuffer
    {
    public:
        DirectX12IndexBuffer(uint32_t *indices, uint32_t count);
        virtual ~DirectX12IndexBuffer();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual uint32_t GetCount() const override { return m_Count; }

    private:
        uint32_t m_Count;
        // DirectX 12 specific members would go here
        ID3D12Resource *m_IndexBuffer;
        D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
    };
} // namespace Platform::DirectX12