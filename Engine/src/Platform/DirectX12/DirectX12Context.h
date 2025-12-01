#pragma once
#include "Engine/Renderer/RendererContext.h"

struct GLFWwindow;
struct ID3D12Device;
struct ID3D12CommandAllocator;
struct ID3D12CommandQueue;
struct ID3D12GraphicsCommandList;
struct IDXGISwapChain3;
struct ID3D12DescriptorHeap;
struct ID3D12Resource;
struct ID3D12Fence;
namespace Platform::DirectX12
{
    class DirectX12Context : public Engine::Renderer::RendererContext
    {
    public:
        DirectX12Context(GLFWwindow *windowHandle);

        virtual void Init() override;
        virtual void SwapBuffers() override;
        virtual void Cleanup() override;
        virtual void SetVSync(bool enabled) override;

        static ID3D12Device *GetDevice() { return s_Instance->m_Device; }
        static ID3D12CommandQueue *GetCommandQueue() { return s_Instance->m_CommandQueue; }
        static ID3D12GraphicsCommandList *GetCommandList() { return s_Instance->m_CommandList; }
        static IDXGISwapChain3 *GetSwapChain() { return s_Instance->m_SwapChain; }
        static ID3D12DescriptorHeap *GetRTVHeap() { return s_Instance->m_RTVHeap; }

    private:
        bool CreateDevice();
        bool CreateCommandQueue();
        bool CreateCommandAllocator();
        bool CreateCommandList();
        bool CreateSwapChain();
        bool CreateRenderTargetView();
        bool CreateFence();
        void WaitForPreviousFrame();

    private:
        GLFWwindow *m_WindowHandle;
        uint32_t m_Width, m_Height;

        // DirectX 12 specific members
        ID3D12Device *m_Device = nullptr;
        ID3D12CommandAllocator *m_CommandAllocator = nullptr;
        ID3D12CommandQueue *m_CommandQueue = nullptr;
        ID3D12GraphicsCommandList *m_CommandList = nullptr;
        IDXGISwapChain3 *m_SwapChain = nullptr;
        ID3D12DescriptorHeap *m_RTVHeap = nullptr;
        ID3D12Resource *m_RenderTargets[2] = {nullptr, nullptr};
        ID3D12Fence *m_Fence = nullptr;
        HANDLE m_FenceEvent = nullptr;
        UINT64 m_FenceValue = 0;

        static const uint32_t FrameCount = 2;
        uint32_t m_FrameIndex = 0;

        bool m_VSyncEnabled = false;

        static DirectX12Context *s_Instance;
    };
} // namespace
