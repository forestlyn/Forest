#include "DirectX12RenderAPI.h"
#include "DirectX12Context.h"
#include "Engine/pcheader.h"
#include <directx/d3d12.h>
#include <directx/d3dx12.h>
#include <dxgi1_4.h>

namespace Platform::DirectX12
{
    void DirectX12RenderAPI::SetClearColor(const glm::vec4 &color)
    {
        // DirectX 12 specific clear color setting would go here
        // Typically, this would involve storing the clear color for use during the Clear operation
        m_ClearColor = color;
    }

    void DirectX12RenderAPI::Clear()
    {
        // DirectX 12 specific clear operation would go here
        ID3D12GraphicsCommandList *commandList = DirectX12Context::GetCommandList();
        ID3D12DescriptorHeap *rtvHeap = DirectX12Context::GetRTVHeap();
        UINT rtvDescriptorSize = DirectX12Context::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(),
                                                DirectX12Context::GetSwapChain()->GetCurrentBackBufferIndex(), rtvDescriptorSize);

        FLOAT clearColor[4] = {m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a};
        commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    }

    void DirectX12RenderAPI::DrawIndexed(std::shared_ptr<Engine::Renderer::VertexArray> &vertexArray)
    {
        // DirectX 12 specific draw indexed operation would go here
        ID3D12GraphicsCommandList *commandList = DirectX12Context::GetCommandList();

        vertexArray->Bind();
        uint32_t indexCount = vertexArray->GetIndexBuffer()->GetCount();
        commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
    }
} // namespace Platform::DirectX12
