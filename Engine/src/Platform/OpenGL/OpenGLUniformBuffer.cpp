
#include "Engine/pcheader.h"
#include "OpenGLUniformBuffer.h"
#include "Engine/Profile/Instrumentor.h"
#include <glad/glad.h>
namespace Engine::Platform::OpenGL
{
    OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
    {
        ENGINE_PROFILING_FUNC();

        glCreateBuffers(1, &m_RendererID);
        glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
    }

    OpenGLUniformBuffer::~OpenGLUniformBuffer()
    {
        ENGINE_PROFILING_FUNC();

        glDeleteBuffers(1, &m_RendererID);
    }

    void OpenGLUniformBuffer::SetData(const void *data, uint32_t size, uint32_t offset)
    {
        ENGINE_PROFILING_FUNC();

        glNamedBufferSubData(m_RendererID, offset, size, data);
    }
} // namespace Engine::Platform::OpenGL