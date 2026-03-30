#include "OpenGLTexture.h"
#include <glad/glad.h>
#include "stb_image.h"
#include "Engine/Profile/Instrumentor.h"
namespace Platform::OpenGL
{
    OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height)
    {
        ENGINE_PROFILING_FUNC();
        internalFormat = GL_RGBA8;
    }

    OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, int channels, void *data) : m_Width(width), m_Height(height)
    {
        if (channels == 4)
        {
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
        }
        else if (channels == 3)
        {
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
        }
        data = data;
    }

    void OpenGLTexture2D::Init()
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

        if (data)
            glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        ENGINE_PROFILING_FUNC();

        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLTexture2D::Bind(uint32_t slot) const
    {
        ENGINE_PROFILING_FUNC();

        glBindTextureUnit(slot, m_RendererID);
    }

    void OpenGLTexture2D::SetData(void *data, uint32_t size)
    {
        ENGINE_PROFILING_FUNC();

        uint32_t bpp = (m_Width * m_Height * 4); // Assuming 4 bytes per pixel (RGBA)
        ENGINE_ASSERT(size == bpp, "Data must be entire texture!");
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
} // namespace Platform::OpenGL