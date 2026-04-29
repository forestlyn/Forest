#pragma once
#include "Engine/Renderer/FrameBuffer.h"
#include <cstdint>
#include <mutex>

namespace Platform::OpenGL
{
    class OpenGLFrameBuffer : public Engine::Renderer::FrameBuffer
    {
    public:
        OpenGLFrameBuffer(const Engine::Renderer::FrameBufferSpecification &spec);
        virtual ~OpenGLFrameBuffer();

        virtual void Init() override;

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void Resize(uint32_t width, uint32_t height) override;

        virtual int GetColorAttachmentRendererID(int index = 0) const override { return m_ColorAttachments[index]; }
        virtual uint64_t RequestPixelData(int index, int x, int y) override;
        virtual bool TryGetPixelData(uint64_t requestID, int &outPixelData) const override;
        void ClearAttachment(int index, int value) override;

        virtual const Engine::Renderer::FrameBufferSpecification &GetSpecification() const override { return m_Spec; }

    private:
        void Invalidate();

    private:
        Engine::Renderer::FrameBufferSpecification m_Spec;

        uint32_t m_RendererID = 0;

        std::vector<Engine::Renderer::FrameBufferTextureSpecification> m_ColorAttachmentSpecifications;
        Engine::Renderer::FrameBufferTextureSpecification m_DepthAttachmentSpecification = Engine::Renderer::TextureFormat::None;

        std::vector<uint32_t> m_ColorAttachments;
        uint32_t m_DepthAttachment = 0;

        uint64_t m_NextPixelReadRequestID = 1;
        mutable std::mutex m_PixelReadbackMutex;
        uint64_t m_CompletedPixelReadRequestID = 0;
        int m_CompletedPixelData = -1;
    };
}
