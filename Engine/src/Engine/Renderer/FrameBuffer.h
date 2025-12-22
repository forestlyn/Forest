#pragma once
#include <glm/glm.hpp>

namespace Engine::Renderer
{
    struct FrameBufferSpecification
    {
        uint32_t Width = 1280;
        uint32_t Height = 720;

        uint32_t Samples = 1;
        bool SwapChainTarget = false;
    };

    class FrameBuffer
    {
    public:
        virtual ~FrameBuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual int GetColorAttachmentRendererID() const = 0;

        virtual const FrameBufferSpecification &GetSpecification() const = 0;

        static Ref<FrameBuffer> Create(const FrameBufferSpecification &spec);
    };

} // namespace Engine::Renderer
