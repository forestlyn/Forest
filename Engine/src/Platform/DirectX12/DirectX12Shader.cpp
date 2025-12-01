#include "Engine/pcheader.h"
#include "DirectX12Shader.h"
#include <directx/d3d12.h>
#include <d3dcompiler.h>
#include "DirectX12Context.h"
#include <directx/d3dx12.h>
namespace Platform::DirectX12
{
    DirectX12Shader::DirectX12Shader(const std::string &vertexSrc, const std::string &fragmentSrc)
    {
        // DirectX 12 specific shader compilation would go here]
        // 编译着色器
        ID3DBlob *vertexShader;
        ID3DBlob *pixelShader;
        ID3DBlob *errorBlob;

        const char *vsCode = vertexSrc.c_str();
        const char *psCode = fragmentSrc.c_str();

        HRESULT hr = D3DCompile(
            vsCode, strlen(vsCode), nullptr, nullptr, nullptr,
            "main", "vs_5_0", 0, 0, &vertexShader, &errorBlob);

        ENGINE_ASSERT(SUCCEEDED(hr), "Failed to compile vertex shader!:{0}", errorBlob ? (char *)errorBlob->GetBufferPointer() : "");

        hr = D3DCompile(
            psCode, strlen(psCode), nullptr, nullptr, nullptr,
            "main", "ps_5_0", 0, 0, &pixelShader, &errorBlob);
        ENGINE_ASSERT(SUCCEEDED(hr), "Failed to compile pixel shader!:{0}", errorBlob ? (char *)errorBlob->GetBufferPointer() : "");

        // 定义顶点输入布局
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
            {
                {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            };

        // 创建空的根签名
        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        ID3DBlob *signature;
        hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &errorBlob);
        ENGINE_ASSERT(SUCCEEDED(hr), "Failed to serialize root signature!:{0}", errorBlob ? (char *)errorBlob->GetBufferPointer() : "");

        ID3D12Device *device = DirectX12Context::GetDevice();
        hr = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
        signature->Release();

        ENGINE_ASSERT(SUCCEEDED(hr), "Failed to create root signature!");

        // 创建图形管道状态
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = {inputElementDescs, _countof(inputElementDescs)};
        psoDesc.pRootSignature = m_RootSignature;
        psoDesc.VS = {vertexShader->GetBufferPointer(), vertexShader->GetBufferSize()};
        psoDesc.PS = {pixelShader->GetBufferPointer(), pixelShader->GetBufferSize()};
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;

        hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState));

        vertexShader->Release();
        pixelShader->Release();

        ENGINE_ASSERT(SUCCEEDED(hr), "Failed to create pipeline state!");
    }

    DirectX12Shader::~DirectX12Shader()
    {
        // DirectX 12 specific shader cleanup would go here
        m_PipelineState->Release();
        m_RootSignature->Release();
    }

    void DirectX12Shader::Bind() const
    {
        // DirectX 12 specific bind code would go here
        ID3D12GraphicsCommandList *commandList = DirectX12Context::GetCommandList();
        commandList->SetPipelineState(m_PipelineState);
    }

    void DirectX12Shader::Unbind() const
    {
        // DirectX 12 specific unbind code would go here
    }
} // namespace Platform::DirectX12