#pragma once
#include "Engine/Renderer/Shader.h"
struct ID3D12PipelineState;
struct ID3D12RootSignature;
namespace Platform::DirectX12
{
    class DirectX12Shader : public Engine::Renderer::Shader
    {
    public:
        DirectX12Shader(const std::string &vertexSrc, const std::string &fragmentSrc);
        virtual ~DirectX12Shader();

        virtual void Bind() const override;
        virtual void Unbind() const override;

    private:
        // DirectX 12 specific members would go here
        ID3D12PipelineState *m_PipelineState;
        ID3D12RootSignature *m_RootSignature;
    };
} // namespace Platform::DirectX12