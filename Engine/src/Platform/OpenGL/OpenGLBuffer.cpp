#include "OpenGLBuffer.h"
#include "glad/glad.h"
#include "Engine/Profile/Instrumentor.h"
namespace Platform::OpenGL
{

    //// VertexBuffer ////

    OpenGLVertexBuffer::OpenGLVertexBuffer(float *vertices, uint32_t size)
    {
        ENGINE_PROFILING_FUNC();

        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    }

    OpenGLVertexBuffer::~OpenGLVertexBuffer()
    {
        ENGINE_PROFILING_FUNC();

        glDeleteBuffers(1, &m_RendererID);
    }

    void OpenGLVertexBuffer::Bind() const
    {
        ENGINE_PROFILING_FUNC();

        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    }

    void OpenGLVertexBuffer::Unbind() const
    {
        ENGINE_PROFILING_FUNC();

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    //// IndexBuffer ////

    OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t *indices, uint32_t count)
        : m_Count(count)
    {
        ENGINE_PROFILING_FUNC();

        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
    }

    OpenGLIndexBuffer::~OpenGLIndexBuffer()
    {
        ENGINE_PROFILING_FUNC();

        glDeleteBuffers(1, &m_RendererID);
    }

    void OpenGLIndexBuffer::Bind() const
    {
        ENGINE_PROFILING_FUNC();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    }

    void OpenGLIndexBuffer::Unbind() const
    {
        ENGINE_PROFILING_FUNC();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

} // namespace Platform::OpenGL
