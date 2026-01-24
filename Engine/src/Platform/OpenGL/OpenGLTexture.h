#pragma once
#include "Engine/Renderer/Shader/Texture.h"

namespace Platform::OpenGL
{
    class OpenGLTexture2D : public Engine::Renderer::Texture2D
    {
    public:
        OpenGLTexture2D(uint32_t width, uint32_t height);
        OpenGLTexture2D(const std::string &path);
        virtual ~OpenGLTexture2D();

        virtual uint32_t GetWidth() const override { return m_Width; }
        virtual uint32_t GetHeight() const override { return m_Height; }

        virtual void Bind(uint32_t slot = 0) const override;
        virtual void SetData(void *data, uint32_t size) override;
        virtual uint32_t GetRendererID() const override { return m_RendererID; }

        virtual bool operator==(const Engine::Renderer::Texture &other) const override
        {
            // ENGINE_INFO("Comparing OpenGLTexture2D instances: {0} and {1}", m_RendererID, ((OpenGLTexture2D &)other).m_RendererID);
            return m_RendererID == other.GetRendererID();
        }

    private:
        std::string m_Path;
        uint32_t m_Width, m_Height;
        uint32_t m_RendererID;
    };
} // namespace Platform::OpenGL