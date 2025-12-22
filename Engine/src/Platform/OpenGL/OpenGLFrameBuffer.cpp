#include "OpenGLFrameBuffer.h"
#include "Engine/Profile/Instrumentor.h"
#include <glad/glad.h>

namespace Platform::OpenGL
{
    OpenGLFrameBuffer::OpenGLFrameBuffer(const Engine::Renderer::FrameBufferSpecification &spec)
        : m_Spec(spec), m_Width(spec.Width), m_Height(spec.Height)
    {
        ENGINE_PROFILING_FUNC();
        Invalidate();
    }

    OpenGLFrameBuffer::~OpenGLFrameBuffer()
    {
        ENGINE_PROFILING_FUNC();
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures(1, &m_ColorAttachment);
    }

    void OpenGLFrameBuffer::Bind() const
    {
        ENGINE_PROFILING_FUNC();
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glViewport(0, 0, m_Width, m_Height);
    }

    void OpenGLFrameBuffer::Unbind() const
    {
        ENGINE_PROFILING_FUNC();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
    {
        ENGINE_PROFILING_FUNC();
        if (width == 0 || height == 0 || width > 8192 || height > 8192)
        {
            ENGINE_WARN("Attempted to resize FrameBuffer to invalid size: {0}, {1}", width, height);
            return;
        }
        m_Width = width;
        m_Height = height;

        glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Width, m_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
    }

    void OpenGLFrameBuffer::Invalidate()
    {

        glCreateFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

        glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

        glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
        glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Width, m_Height);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

        ENGINE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "FrameBuffer is incomplete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
} // namespace Platform::OpenGL