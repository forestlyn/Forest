#pragma once
#include <glm/glm.hpp>

namespace Engine::Renderer
{
    enum class TextureFormat
    {
        None = 0,

        // Color
        RGBA8,

        // Depth/Stencil
        DEPTH24STENCIL8,

        // Defaults
        Depth = DEPTH24STENCIL8
    };
    struct FrameBufferTextureSpecification
    {
        FrameBufferTextureSpecification() = default;
        FrameBufferTextureSpecification(TextureFormat format)
            : Format(format) {}

        TextureFormat Format = TextureFormat::None;
    };

    struct FrameBufferSpecification
    {
        uint32_t Width = 1280;
        uint32_t Height = 720;

        uint32_t Samples = 1;

        bool SwapChainTarget = false;

        std::vector<FrameBufferTextureSpecification> Attachments;
        FrameBufferSpecification() = default;
        FrameBufferSpecification(const std::initializer_list<FrameBufferTextureSpecification> &attachments)
            : Attachments(attachments) {}
    };

    class FrameBuffer
    {
    public:
        virtual ~FrameBuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual int GetColorAttachmentRendererID(int index = 0) const = 0;

        virtual const FrameBufferSpecification &GetSpecification() const = 0;

        static Ref<FrameBuffer> Create(const FrameBufferSpecification &spec);
    };

} // namespace Engine::Renderer
