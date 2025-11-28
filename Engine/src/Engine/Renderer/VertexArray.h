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

        virtual void AddVertexBuffer(const std::shared_ptr<class VertexBuffer> &vertexBuffer) = 0;
        virtual void SetIndexBuffer(const std::shared_ptr<class IndexBuffer> &indexBuffer) = 0;

        virtual const std::vector<std::shared_ptr<class VertexBuffer>> &GetVertexBuffers() const = 0;
        virtual const std::shared_ptr<class IndexBuffer> &GetIndexBuffer() const = 0;

        static VertexArray *Create();
    };
} // namespace Engine::Renderer