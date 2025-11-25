#pragma once
#include "Engine/pcheader.h"
#include "Engine/Renderer/Buffer.h"

namespace Platform::OpenGL
{
    class OpenGLVertexBuffer : public Engine::Renderer::VertexBuffer
    {
    public:
        OpenGLVertexBuffer(float *vertices, unsigned int size);
        virtual ~OpenGLVertexBuffer();

        virtual void Bind() const override;
        virtual void Unbind() const override;

    private:
        unsigned int m_RendererID;
    };

    class OpenGLIndexBuffer : public Engine::Renderer::IndexBuffer
    {
    public:
        OpenGLIndexBuffer(unsigned int *indices, unsigned int count);
        virtual ~OpenGLIndexBuffer();

        virtual void Bind() const override;
        virtual void Unbind() const override;
        virtual unsigned int GetCount() const override { return m_Count; }

    private:
        unsigned int m_RendererID;
        unsigned int m_Count;
    };
} // namespace Platform::OpenGL