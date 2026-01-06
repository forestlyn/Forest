#include "Utils.h"
namespace Platform::OpenGL::Utils
{
    GLenum TextureTarget(bool multisample)
    {
        return multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    }

    GLenum TextureFormatToGLFormat(Engine::Renderer::TextureFormat format)
    {
        switch (format)
        {
        case Engine::Renderer::TextureFormat::RGBA8:
            return GL_RGBA8;
        case Engine::Renderer::TextureFormat::RED_INTEGER_32:
            return GL_R32I;
        case Engine::Renderer::TextureFormat::DEPTH24STENCIL8:
            return GL_DEPTH24_STENCIL8;
        default:
            ENGINE_ASSERT(false, "Unknown TextureFormat!");
            return 0;
        }
    }

    GLenum TextureFormatToGLBaseFormat(Engine::Renderer::TextureFormat format)
    {
        switch (format)
        {
        case Engine::Renderer::TextureFormat::RGBA8:
            return GL_RGBA;
        case Engine::Renderer::TextureFormat::RED_INTEGER_32:
            return GL_RED_INTEGER;
        case Engine::Renderer::TextureFormat::DEPTH24STENCIL8:
            return GL_DEPTH_STENCIL;
        default:
            ENGINE_ASSERT(false, "Unknown TextureFormat!");
            return 0;
        }
    }

    bool IsDepthFormat(Engine::Renderer::TextureFormat format)
    {
        switch (format)
        {
        case Engine::Renderer::TextureFormat::DEPTH24STENCIL8:
            return true;
        default:
            return false;
        }
        return false;
    }

    void GLCreateTexture(bool multisample, uint32_t &outID, uint32_t count)
    {
        glCreateTextures(TextureTarget(multisample), count, &outID);
    }

    void GLBindTexture(bool multisample, uint32_t id)
    {
        glBindTexture(TextureTarget(multisample), id);
    }

    void GLAttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
    {
        if (samples > 1)
        {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

            GLint filter = (format == GL_RED_INTEGER) ? GL_NEAREST : GL_LINEAR;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(samples > 1), id, 0);
    }
    void GLAttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
    {
        if (samples > 1)
        {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
        }
        else
        {
            glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(samples > 1), id, 0);
    }
}