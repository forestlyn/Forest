#pragma once
#include "DirectX12VertexArray.h"
#include "DirectX12Buffer.h"
#include "Engine/pcheader.h"
namespace Platform::DirectX12
{
    DXGI_FORMAT ShaderDataTypeToDXGIFormat(Engine::Renderer::ShaderDataType type)
    {
        switch (type)
        {
        case Engine::Renderer::ShaderDataType::Float:
            return DXGI_FORMAT_R32_FLOAT;
        case Engine::Renderer::ShaderDataType::Float2:
            return DXGI_FORMAT_R32G32_FLOAT;
        case Engine::Renderer::ShaderDataType::Float3:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case Engine::Renderer::ShaderDataType::Float4:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case Engine::Renderer::ShaderDataType::Mat3:
            return DXGI_FORMAT_R32G32B32_FLOAT; // 注意：矩阵需要特殊处理
        case Engine::Renderer::ShaderDataType::Mat4:
            return DXGI_FORMAT_R32G32B32A32_FLOAT; // 注意：矩阵需要特殊处理
        case Engine::Renderer::ShaderDataType::Int:
            return DXGI_FORMAT_R32_SINT;
        case Engine::Renderer::ShaderDataType::Int2:
            return DXGI_FORMAT_R32G32_SINT;
        case Engine::Renderer::ShaderDataType::Int3:
            return DXGI_FORMAT_R32G32B32_SINT;
        case Engine::Renderer::ShaderDataType::Int4:
            return DXGI_FORMAT_R32G32B32A32_SINT;
        case Engine::Renderer::ShaderDataType::Bool:
            return DXGI_FORMAT_R8_UINT;
        }

        ENGINE_ASSERT(false, "Unknown ShaderDataType!");
        return DXGI_FORMAT_UNKNOWN;
    }

    DirectX12VertexArray::DirectX12VertexArray()
    {
    }

    DirectX12VertexArray::~DirectX12VertexArray()
    {
    }

    void DirectX12VertexArray::Bind() const
    {
        // 绑定顶点数组相关的资源
        // DirectX 12不直接支持VAO，需要绑定顶点缓冲区和索引缓冲区

        for (const auto &vertexBuffer : m_VertexBuffers)
        {
            vertexBuffer->Bind();
        }

        if (m_IndexBuffer)
        {
            m_IndexBuffer->Bind();
        }
    }

    void DirectX12VertexArray::Unbind() const
    {
        // DirectX 12不需要显式解绑
    }

    void DirectX12VertexArray::AddVertexBuffer(const std::shared_ptr<Engine::Renderer::VertexBuffer> &vertexBuffer)
    {
        ENGINE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

        // Convert Engine::Renderer::BufferLayout to D3D12_INPUT_ELEMENT_DESC

        const auto &layout = vertexBuffer->GetLayout();
        uint32_t semanticIndex = 0;
        for (const auto &element : layout)
        {
            D3D12_INPUT_ELEMENT_DESC desc = {};
            desc.SemanticName = element.Name.c_str();
            desc.SemanticIndex = semanticIndex++;
            desc.Format = ShaderDataTypeToDXGIFormat(element.Type); // 需要根据element.Type设置正确的格式
            desc.InputSlot = static_cast<UINT>(m_VertexBuffers.size());
            desc.AlignedByteOffset = element.Offset;
            desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            desc.InstanceDataStepRate = 0;

            m_InputLayout.push_back(desc);
        }
        m_VertexBuffers.push_back(vertexBuffer);
    }

    void DirectX12VertexArray::SetIndexBuffer(const std::shared_ptr<Engine::Renderer::IndexBuffer> &indexBuffer)
    {
        m_IndexBuffer = indexBuffer;
    }
} // namespace Platform::DirectX12