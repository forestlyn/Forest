#include "OpenGLTexture.h"
#include <glad/glad.h>
#include "stb_image.h"
namespace Platform::OpenGL
{
    OpenGLTexture2D::OpenGLTexture2D(const std::string &path)
        : m_Path(path)
    {
        // Load texture from file (implementation not shown)
        // Set m_Width, m_Height, and m_RendererID accordingly
        stbi_set_flip_vertically_on_load(1);
        int width, height, channels;
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        ENGINE_ASSERT(data, "Failed to load texture image!");
        m_Width = width;
        m_Height = height;

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        ENGINE_INFO("Loaded texture: {0} (Width: {1}, Height: {2}, Channels: {3})", path, m_Width, m_Height, channels);
        glTextureStorage2D(m_RendererID, 1, GL_RGB8, m_Width, m_Height);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, GL_RGB, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLTexture2D::Bind(uint32_t slot) const
    {
        // Bind texture to specified slot (implementation not shown)
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }
} // namespace Platform::OpenGL