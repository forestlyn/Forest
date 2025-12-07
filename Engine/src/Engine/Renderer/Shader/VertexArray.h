#pragma once
#include <memory>
#include <vector>
#include "Buffer.h"
namespace Engine::Renderer
{
    class VertexArray
    {
    public:
        virtual ~VertexArray();

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void AddVertexBuffer(const Ref<class VertexBuffer> &vertexBuffer) = 0;
        virtual void SetIndexBuffer(const Ref<class IndexBuffer> &indexBuffer) = 0;

        virtual const std::vector<Ref<class VertexBuffer>> &GetVertexBuffers() const = 0;
        virtual const Ref<class IndexBuffer> &GetIndexBuffer() const = 0;

        static Ref<VertexArray> Create();
    };
} // namespace Engine::Renderer