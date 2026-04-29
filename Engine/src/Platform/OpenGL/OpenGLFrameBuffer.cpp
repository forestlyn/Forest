#include "OpenGLFrameBuffer.h"
#include "Engine/Profile/Instrumentor.h"
#include "Engine/Core/Application.h"
#include <glad/glad.h>
#include "Utils.h"
namespace Platform::OpenGL
{
    OpenGLFrameBuffer::OpenGLFrameBuffer(const Engine::Renderer::FrameBufferSpecification &spec)
        : m_Spec(spec)
    {
        ENGINE_PROFILING_FUNC();
        for (size_t i = 0; i < m_Spec.Attachments.size(); i++)
        {
            const auto &spec = m_Spec.Attachments[i];
            if (!Utils::IsDepthFormat(spec.Format))
                m_ColorAttachmentSpecifications.push_back(spec);
            else
                m_DepthAttachmentSpecification = spec;
        }
    }

    void OpenGLFrameBuffer::Init()
    {
        Invalidate();
    }

    OpenGLFrameBuffer::~OpenGLFrameBuffer()
    {
        ENGINE_PROFILING_FUNC();
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
        glDeleteTextures(1, &m_DepthAttachment);
    }

    void OpenGLFrameBuffer::Bind() const
    {
        ENGINE_PROFILING_FUNC();
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glViewport(0, 0, m_Spec.Width, m_Spec.Height);
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
        m_Spec.Width = width;
        m_Spec.Height = height;

        ENQUEUE_RENDER_COMMAND(this)
        this->Invalidate();
        ENQUEUE_RENDER_COMMAND_END()
    }

    uint64_t OpenGLFrameBuffer::RequestPixelData(int index, int x, int y)
    {
        ENGINE_PROFILING_FUNC();
        ENGINE_ASSERT(index < m_ColorAttachmentSpecifications.size(), "Color attachment index out of bounds!");

        if (index < 0 || index >= m_ColorAttachmentSpecifications.size() || x < 0 || y < 0 ||
            x >= (int)m_Spec.Width || y >= (int)m_Spec.Height)
        {
            return 0;
        }

        uint64_t requestID = m_NextPixelReadRequestID++;

        ENQUEUE_RENDER_COMMAND(this, index, x, y, requestID)
        ENGINE_ASSERT(index < m_ColorAttachments.size(), "Color attachment index out of bounds!");

        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glReadBuffer(GL_COLOR_ATTACHMENT0 + index);

        int pixelData = -1;
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);

        {
            std::scoped_lock<std::mutex> lock(m_PixelReadbackMutex);
            m_CompletedPixelReadRequestID = requestID;
            m_CompletedPixelData = pixelData;
        }
        ENQUEUE_RENDER_COMMAND_END()

        return requestID;
    }

    bool OpenGLFrameBuffer::TryGetPixelData(uint64_t requestID, int &outPixelData) const
    {
        ENGINE_PROFILING_FUNC();
        if (requestID == 0)
            return false;

        std::scoped_lock<std::mutex> lock(m_PixelReadbackMutex);
        if (m_CompletedPixelReadRequestID < requestID)
            return false;

        outPixelData = m_CompletedPixelData;
        return true;
    }

    void OpenGLFrameBuffer::ClearAttachment(int index, int value)
    {
        ENGINE_PROFILING_FUNC();
        ENGINE_ASSERT(index < m_ColorAttachments.size(), "Color attachment index out of bounds!");
        glClearBufferiv(GL_COLOR, index, &value);
    }

    void OpenGLFrameBuffer::Invalidate()
    {
        if (m_RendererID)
        {
            glDeleteFramebuffers(1, &m_RendererID);
            glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
            glDeleteTextures(1, &m_DepthAttachment);
            m_ColorAttachments.clear();
            m_DepthAttachment = 0;
        }

        glCreateFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

        for (int index = 0; index < m_ColorAttachmentSpecifications.size(); index++)
        {
            auto &spec = m_ColorAttachmentSpecifications[index];
            uint32_t colorAttachment;
            Utils::GLCreateTexture(m_Spec.Samples > 1, colorAttachment, 1);
            Utils::GLBindTexture(m_Spec.Samples > 1, colorAttachment);
            Utils::GLAttachColorTexture(colorAttachment, m_Spec.Samples, Utils::TextureFormatToGLFormat(spec.Format),
                                        Utils::TextureFormatToGLBaseFormat(spec.Format), m_Spec.Width, m_Spec.Height, index);
            m_ColorAttachments.push_back(colorAttachment);
        }
        if (Utils::IsDepthFormat(m_DepthAttachmentSpecification.Format))
        {
            Utils::GLCreateTexture(m_Spec.Samples > 1, m_DepthAttachment, 1);
            Utils::GLBindTexture(m_Spec.Samples > 1, m_DepthAttachment);
            Utils::GLAttachDepthTexture(m_DepthAttachment, m_Spec.Samples, Utils::TextureFormatToGLFormat(m_DepthAttachmentSpecification.Format),
                                        GL_DEPTH_STENCIL_ATTACHMENT, m_Spec.Width, m_Spec.Height);
        }

        if (m_ColorAttachments.size() > 1)
        {
            ENGINE_ASSERT(m_ColorAttachments.size() <= 4, "Too many color attachments!");
            GLenum buffers[4];
            for (size_t i = 0; i < m_ColorAttachments.size(); i++)
                buffers[i] = GL_COLOR_ATTACHMENT0 + i;
            glDrawBuffers(m_ColorAttachments.size(), buffers);
        }
        else if (m_ColorAttachments.empty())
        {
            // Only depth-pass
            glDrawBuffer(GL_NONE);
        }

        ENGINE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "FrameBuffer is incomplete!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
} // namespace Platform::OpenGL
