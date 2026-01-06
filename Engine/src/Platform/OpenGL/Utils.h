#pragma once
#include "Engine/Renderer/FrameBuffer.h"
#include <glad/glad.h>
namespace Platform::OpenGL::Utils
{
    GLenum TextureTarget(bool multisample);
    GLenum TextureFormatToGLFormat(Engine::Renderer::TextureFormat format);

    bool IsDepthFormat(Engine::Renderer::TextureFormat format);

    void GLCreateTexture(bool multisample, uint32_t &outID, uint32_t count);
    void GLBindTexture(bool multisample, uint32_t id);
    void GLAttachColorTexture(uint32_t id, int samples, GLenum internalFormat, uint32_t width, uint32_t height, int index);
    void GLAttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height);
}