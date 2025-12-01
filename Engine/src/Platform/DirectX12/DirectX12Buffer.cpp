#include "DirectX12Buffer.h"
#include <directx/d3dx12.h>
#include <dxgi1_4.h>
#include "DirectX12Context.h"
namespace Platform::DirectX12
{
    ////////////////////////////////////////////////////////////////////////
    // DirectX12VertexBuffer
    ///////////////////////////////////////////////////////////////////////
    DirectX12VertexBuffer::DirectX12VertexBuffer(float *vertices, uint32_t size)
    {
        // 创建默认堆中的顶点缓冲区
        CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
        ID3D12Device *device = DirectX12Context::GetDevice();
        HRESULT hr = device->CreateCommittedResource(
            &defaultHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&m_VertexBuffer));

        ENGINE_ASSERT(SUCCEEDED(hr), "Failed to create vertex buffer!");

        // 创建上传堆
        CD3DX12_HEAP_PROPERTIES uploadHeapProps(D3D12_HEAP_TYPE_UPLOAD);
        ID3D12Resource *uploadBuffer;

        hr = device->CreateCommittedResource(
            &uploadHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&uploadBuffer));

        ENGINE_ASSERT(SUCCEEDED(hr), "Failed to create upload buffer!");

        // 复制数据到上传堆
        void *mappedData;
        uploadBuffer->Map(0, nullptr, &mappedData);
        memcpy(mappedData, vertices, size);
        uploadBuffer->Unmap(0, nullptr);

        // 使用命令列表将数据从上传堆复制到默认堆
        ID3D12GraphicsCommandList *commandList = DirectX12Context::GetCommandList();

        commandList->CopyResource(m_VertexBuffer, uploadBuffer);

        // 5. 资源状态转换
        commandList->ResourceBarrier(1,
                                     &CD3DX12_RESOURCE_BARRIER::Transition(
                                         m_VertexBuffer,
                                         D3D12_RESOURCE_STATE_COPY_DEST,
                                         D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

        // 设置顶点缓冲区视图
        m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
        m_VertexBufferView.SizeInBytes = size;
        m_VertexBufferView.StrideInBytes = 0; // 需要根据布局设置
    }

    DirectX12VertexBuffer::~DirectX12VertexBuffer()
    {
        // DirectX 12 specific vertex buffer cleanup code would go here
        if (m_VertexBuffer)
        {
            m_VertexBuffer->Release();
            m_VertexBuffer = nullptr;
        }
    }

    void DirectX12VertexBuffer::Bind() const
    {
        // DirectX 12 specific bind code would go here
        ID3D12GraphicsCommandList *commandList = DirectX12Context::GetCommandList();
        commandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
    }

    void DirectX12VertexBuffer::Unbind() const
    {
        // DirectX 12 specific unbind code would go here
    }

    void DirectX12VertexBuffer::SetLayout(const Engine::Renderer::BufferLayout &layout)
    {
        m_Layout = layout;
        // 更新m_VertexBufferView.StrideInBytes根据新的布局
        m_VertexBufferView.StrideInBytes = m_Layout.GetStride();
    }

    const Engine::Renderer::BufferLayout &DirectX12VertexBuffer::GetLayout() const
    {
        return m_Layout;
    }

    ////////////////////////////////////////////////////////////////////////
    // DirectX12IndexBuffer
    ////////////////////////////////////////////////////////////////////////

    DirectX12IndexBuffer::DirectX12IndexBuffer(uint32_t *indices, uint32_t count)
        : m_Count(count)
    {
        // DirectX 12 specific index buffer creation code would go here
        m_Count = count;
        uint32_t size = count * sizeof(uint32_t);
        auto device = DirectX12Context::GetDevice();
        auto commandList = DirectX12Context::GetCommandList();

        ENGINE_ASSERT(device, "DirectX12 device is not initialized!");
        ENGINE_ASSERT(commandList, "DirectX12 command list is not initialized!");

        // 创建默认堆资源 - 简化版本
        ENGINE_ASSERT(device->CreateCommittedResource(
                          &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                          D3D12_HEAP_FLAG_NONE, // 使用基本的堆标志
                          &CD3DX12_RESOURCE_DESC::Buffer(size),
                          D3D12_RESOURCE_STATE_COPY_DEST,
                          nullptr,
                          IID_PPV_ARGS(&m_IndexBuffer)),
                      "Failed to create index buffer!");

        // 创建上传堆
        ID3D12Resource *uploadBuffer;
        ENGINE_ASSERT(device->CreateCommittedResource(
                          &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                          D3D12_HEAP_FLAG_NONE, // 使用基本的堆标志
                          &CD3DX12_RESOURCE_DESC::Buffer(size),
                          D3D12_RESOURCE_STATE_GENERIC_READ,
                          nullptr,
                          IID_PPV_ARGS(&uploadBuffer)),
                      "Failed to create upload buffer!");

        // 复制数据
        void *mappedData;
        uploadBuffer->Map(0, nullptr, &mappedData);
        memcpy(mappedData, indices, size);
        uploadBuffer->Unmap(0, nullptr);
        // 记录拷贝命令
        commandList->CopyResource(m_IndexBuffer, uploadBuffer);

        // 资源屏障
        commandList->ResourceBarrier(1,
                                     &CD3DX12_RESOURCE_BARRIER::Transition(
                                         m_IndexBuffer,
                                         D3D12_RESOURCE_STATE_COPY_DEST,
                                         D3D12_RESOURCE_STATE_INDEX_BUFFER));

        // 创建视图
        m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
        m_IndexBufferView.SizeInBytes = size;
        m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
    }

    DirectX12IndexBuffer::~DirectX12IndexBuffer()
    {
        // DirectX 12 specific index buffer cleanup code would go here
        if (m_IndexBuffer)
        {
            m_IndexBuffer->Release();
            m_IndexBuffer = nullptr;
        }
    }

    void DirectX12IndexBuffer::Bind() const
    {
        // DirectX 12 specific bind code would go here
        ID3D12GraphicsCommandList *commandList = DirectX12Context::GetCommandList();
        commandList->IASetIndexBuffer(&m_IndexBufferView);
    }

    void DirectX12IndexBuffer::Unbind() const
    {
        // DirectX 12 specific unbind code would go here
    }
} // namespace Platform::DirectX12