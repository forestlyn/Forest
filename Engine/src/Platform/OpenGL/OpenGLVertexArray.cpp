#include "OpenGLVertexArray.h"
#include "Engine/pcheader.h"
#include <glad/glad.h>
namespace Platform::OpenGL
{
    GLenum GetGLenumFromShaderDataType(Engine::Renderer::ShaderDataType type)
    {
        switch (type)
        {
        case Engine::Renderer::ShaderDataType::Float:
        case Engine::Renderer::ShaderDataType::Float2:
        case Engine::Renderer::ShaderDataType::Float3:
        case Engine::Renderer::ShaderDataType::Float4:
        case Engine::Renderer::ShaderDataType::Mat3:
        case Engine::Renderer::ShaderDataType::Mat4:
            return GL_FLOAT;
        case Engine::Renderer::ShaderDataType::Int:
        case Engine::Renderer::ShaderDataType::Int2:
        case Engine::Renderer::ShaderDataType::Int3:
        case Engine::Renderer::ShaderDataType::Int4:
            return GL_INT;
        case Engine::Renderer::ShaderDataType::Bool:
            return GL_BOOL;
        default:
            ENGINE_ASSERT(false, "Unknown ShaderDataType!");
            return 0;
        }
    }

    OpenGLVertexArray::OpenGLVertexArray()
    {
        glCreateVertexArrays(1, &m_RendererID);
    }

    OpenGLVertexArray::~OpenGLVertexArray()
    {
        glDeleteVertexArrays(1, &m_RendererID);
    }

    void OpenGLVertexArray::Bind() const
    {
        glBindVertexArray(m_RendererID);
    }

    void OpenGLVertexArray::Unbind() const
    {
        glBindVertexArray(0);
    }

    void OpenGLVertexArray::AddVertexBuffer(const Engine::Ref<Engine::Renderer::VertexBuffer> &vertexBuffer)
    {
        ENGINE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

        glBindVertexArray(m_RendererID);
        vertexBuffer->Bind();
        const auto &layout = vertexBuffer->GetLayout();
        uint32_t index = 0;
        for (const auto &element : layout)
        {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index,
                                  element.GetComponentCount(),
                                  GetGLenumFromShaderDataType(element.Type),
                                  element.Normalized ? GL_TRUE : GL_FALSE,
                                  layout.GetStride(),
                                  reinterpret_cast<const void *>(element.Offset));
            index++;
        }
        m_VertexBuffers.push_back(vertexBuffer);
    }

    void OpenGLVertexArray::SetIndexBuffer(const Engine::Ref<Engine::Renderer::IndexBuffer> &indexBuffer)
    {
        glBindVertexArray(m_RendererID);
        indexBuffer->Bind();

        m_IndexBuffer = indexBuffer;
    }
} // namespace Platform::OpenGL