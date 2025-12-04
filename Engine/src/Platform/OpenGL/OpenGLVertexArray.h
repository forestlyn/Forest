#pragma once
#include "Engine/Renderer/VertexArray.h"

namespace Platform::OpenGL
{
    class OpenGLVertexArray : public Engine::Renderer::VertexArray
    {
    public:
        OpenGLVertexArray();
        virtual ~OpenGLVertexArray();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void AddVertexBuffer(const Engine::Ref<Engine::Renderer::VertexBuffer> &vertexBuffer) override;
        virtual void SetIndexBuffer(const Engine::Ref<Engine::Renderer::IndexBuffer> &indexBuffer) override;

        virtual const std::vector<Engine::Ref<Engine::Renderer::VertexBuffer>> &GetVertexBuffers() const override
        {
            return m_VertexBuffers;
        }
        virtual const Engine::Ref<Engine::Renderer::IndexBuffer> &GetIndexBuffer() const override
        {
            return m_IndexBuffer;
        }

    private:
        uint32_t m_RendererID;
        std::vector<Engine::Ref<Engine::Renderer::VertexBuffer>> m_VertexBuffers;
        Engine::Ref<Engine::Renderer::IndexBuffer> m_IndexBuffer;
    };
} // namespace Engine::Platform::OpenGL