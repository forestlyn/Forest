#pragma once
#include "Engine/Renderer/UniformBuffer.h"

namespace Engine::Platform::OpenGL
{
    class OpenGLUniformBuffer : public Renderer::UniformBuffer
    {
    public:
        OpenGLUniformBuffer(uint32_t size, uint32_t binding);
        ~OpenGLUniformBuffer() override;

        void SetData(const void *data, uint32_t size, uint32_t offset = 0) override;

    private:
        uint32_t m_RendererID;
    };
} // namespace Engine::Platform::OpenGL