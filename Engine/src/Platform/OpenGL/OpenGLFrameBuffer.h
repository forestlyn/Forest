#pragma once
#include "Engine/Renderer/FrameBuffer.h"

namespace Platform::OpenGL
{
    class OpenGLFrameBuffer : public Engine::Renderer::FrameBuffer
    {
    public:
        OpenGLFrameBuffer(const Engine::Renderer::FrameBufferSpecification &spec);
        virtual ~OpenGLFrameBuffer();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void Resize(uint32_t width, uint32_t height) override;

        virtual int GetColorAttachmentRendererID(int index = 0) const override { return m_ColorAttachments[index]; }

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
    };
}