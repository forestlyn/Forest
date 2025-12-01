#include "DirectX12Context.h"
#include "Engine/pcheader.h"
#include "glfw/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <d3d12.h>
#include <dxgi1_4.h>
namespace Platform::DirectX12
{
    DirectX12Context *DirectX12Context::s_Instance = nullptr;

    DirectX12Context::DirectX12Context(GLFWwindow *windowHandle)
        : m_WindowHandle(windowHandle)
    {
        ENGINE_ASSERT(!s_Instance, "DirectX12Context instance already exists!");
        s_Instance = this;
        ENGINE_ASSERT(windowHandle, "Window handle is null!");
        int width, height;
        glfwGetWindowSize(windowHandle, &width, &height);
        m_Width = static_cast<uint32_t>(width);
        m_Height = static_cast<uint32_t>(height);
        m_FrameIndex = 0;
        m_VSyncEnabled = false;
        m_FenceValue = 0;
        m_FenceEvent = nullptr;
    }

    void DirectX12Context::Init()
    {
        // 初始化DX12
        ENGINE_ASSERT(CreateDevice(), "Failed to create D3D12 device!");
        ENGINE_ASSERT(CreateCommandQueue(), "Failed to create command queue!");
        ENGINE_ASSERT(CreateCommandAllocator(), "Failed to create command allocator!");
        ENGINE_ASSERT(CreateCommandList(), "Failed to create command list!");
        ENGINE_ASSERT(CreateSwapChain(), "Failed to create swap chain!");
        ENGINE_ASSERT(CreateRenderTargetView(), "Failed to create render target view!");
        ENGINE_ASSERT(CreateFence(), "Failed to create fence!");

        ENGINE_INFO("DirectX 12 Context Initialized:");
        ENGINE_INFO("  Vendor: Microsoft");
        ENGINE_INFO("  Renderer: DirectX 12");
        ENGINE_INFO("  Version: 12.0");
        ENGINE_INFO("  Back Buffer Count: {0}", FrameCount);
        ENGINE_INFO("  VSync: {0}", m_VSyncEnabled ? "Enabled" : "Disabled");
    }

    void DirectX12Context::SwapBuffers()
    {
        // Present - VSync控制在这里实现
        UINT syncInterval = m_VSyncEnabled ? 1 : 0;
        ENGINE_ASSERT(m_SwapChain->Present(syncInterval, 0), "Failed to present swap chain!");

        // 更新帧索引并等待
        m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

        // 信号围栏并等待
        m_FenceValue++;
        ENGINE_ASSERT(m_CommandQueue->Signal(m_Fence, m_FenceValue), "Failed to signal command queue!");

        if (m_Fence->GetCompletedValue() < m_FenceValue)
        {
            ENGINE_ASSERT(m_Fence->SetEventOnCompletion(m_FenceValue, m_FenceEvent), "Failed to set event on completion!");
            WaitForSingleObject(m_FenceEvent, INFINITE);
        }
    }

    void DirectX12Context::SetVSync(bool enabled)
    {
        m_VSyncEnabled = enabled;
    }

    void DirectX12Context::Cleanup()
    {
        if (m_FenceEvent)
        {
            CloseHandle(m_FenceEvent);
            m_FenceEvent = nullptr;
        }
        if (m_Fence)
        {
            m_Fence->Release();
            m_Fence = nullptr;
        }
        for (UINT i = 0; i < FrameCount; i++)
        {
            if (m_RenderTargets[i])
            {
                m_RenderTargets[i]->Release();
                m_RenderTargets[i] = nullptr;
            }
        }
        if (m_RTVHeap)
        {
            m_RTVHeap->Release();
            m_RTVHeap = nullptr;
        }
        if (m_Device)
        {
            m_Device->Release();
            m_Device = nullptr;
        }
    }

    bool DirectX12Context::CreateDevice()
    {
        // 启用调试层（仅在调试模式下）
#ifdef _DEBUG
        ID3D12Debug *debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
            debugController->Release();
        }
#endif

        // 创建设备
        HRESULT hr = D3D12CreateDevice(
            nullptr,
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&m_Device));

        return SUCCEEDED(hr);
    }

    bool DirectX12Context::CreateCommandQueue()
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

        HRESULT hr = m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue));
        return SUCCEEDED(hr);
    }

    bool DirectX12Context::CreateCommandAllocator()
    {
        HRESULT hr = m_Device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT, // 与命令列表类型一致
            IID_PPV_ARGS(&m_CommandAllocator));

        if (FAILED(hr))
        {
            OutputDebugStringA("Failed to create command allocator!");
            return false;
        }

        return true;
    }

    bool DirectX12Context::CreateCommandList()
    {
        HRESULT hr = m_Device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            /* Command Allocator */ m_CommandAllocator,
            /* Initial Pipeline State */ nullptr,
            IID_PPV_ARGS(&m_CommandList));

        if (SUCCEEDED(hr))
        {
            m_CommandList->Close(); // 初始状态下关闭命令列表
        }

        return SUCCEEDED(hr);
    }

    bool DirectX12Context::CreateSwapChain()
    {
        ENGINE_ASSERT(!m_SwapChain, "m_SwapChain has created");
        ENGINE_ASSERT(m_Device, "Device not created!");
        ENGINE_ASSERT(m_CommandQueue, "Command Queue not created!");
        ENGINE_ASSERT(m_WindowHandle, "Window handle is invalid!");
        ENGINE_ASSERT(m_Width > 0 && m_Height > 0, "Window size is zero!");

        IDXGIFactory4 *factory;
        CreateDXGIFactory1(IID_PPV_ARGS(&factory));

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.BufferCount = FrameCount;
        swapChainDesc.Width = m_Width;
        swapChainDesc.Height = m_Height;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.SampleDesc.Count = 1;

        HWND hwnd = glfwGetWin32Window(m_WindowHandle);
        ENGINE_ASSERT(hwnd, "Failed to get HWND from GLFW window! (forgot GLFW_EXPOSE_NATIVE_WIN32?)");
        IDXGISwapChain1 *tempSwapChain;
        HRESULT hr = factory->CreateSwapChainForHwnd(
            m_CommandQueue,
            hwnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            &tempSwapChain);
        ENGINE_ASSERT(SUCCEEDED(hr), "CreateSwapChainForHwnd fail!");
        if (SUCCEEDED(hr))
        {
            m_SwapChain = static_cast<IDXGISwapChain3 *>(tempSwapChain);
            m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
            ENGINE_INFO("m_SwapChain set");
        }

        return SUCCEEDED(hr);
    }

    bool DirectX12Context::CreateRenderTargetView()
    {
        // 创建RTV描述符堆
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = FrameCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        HRESULT hr = m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RTVHeap));
        if (FAILED(hr))
            return false;

        // 创建RTV
        UINT rtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTVHeap->GetCPUDescriptorHandleForHeapStart();

        for (uint32_t i = 0; i < FrameCount; i++)
        {
            hr = m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i]));
            if (FAILED(hr))
                return false;

            m_Device->CreateRenderTargetView(m_RenderTargets[i], nullptr, rtvHandle);
            rtvHandle.ptr += rtvDescriptorSize;
        }

        return true;
    }

    bool DirectX12Context::CreateFence()
    {
        HRESULT hr = m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
        if (FAILED(hr))
            return false;

        m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        return m_FenceEvent != nullptr;
    }

    void DirectX12Context::WaitForPreviousFrame()
    {
        if (m_CommandQueue && m_Fence && m_FenceEvent)
        {
            const UINT64 fence = m_FenceValue;
            ENGINE_ASSERT(m_CommandQueue->Signal(m_Fence, fence), "Failed to signal command queue!");
            m_FenceValue++;

            if (m_Fence->GetCompletedValue() < fence)
            {
                ENGINE_ASSERT(m_Fence->SetEventOnCompletion(fence, m_FenceEvent), "Failed to set event on completion!");
                WaitForSingleObject(m_FenceEvent, INFINITE);
            }
        }
    }

} // namespace Platform::DirectX12