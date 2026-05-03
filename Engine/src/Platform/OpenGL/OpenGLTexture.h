#pragma once
#include "Engine/Renderer/Shader/Texture.h"

namespace Platform::OpenGL
{
    class OpenGLTexture2D : public Engine::Renderer::Texture2D
    {
    public:
        OpenGLTexture2D(uint32_t width, uint32_t height);
        OpenGLTexture2D(uint32_t width, uint32_t height, int channels, void *data, const std::string &path);
        virtual ~OpenGLTexture2D();

        virtual void Init() override;

        virtual uint32_t GetWidth() const override { return m_Width; }
        virtual uint32_t GetHeight() const override { return m_Height; }

        virtual std::string GetPath() const override { return m_Path; }

        virtual void Bind(uint32_t slot = 0) const override;
        virtual void SetData(void *data, uint32_t size) override;
        virtual uint32_t GetRendererID() const override { return m_RendererID; }

        virtual bool operator==(const Engine::Renderer::Texture &other) const override
        {
            if (this == &other)
                return true;

            if (!m_Path.empty() && m_Path == other.GetPath())
                return true;

            uint32_t otherRendererID = other.GetRendererID();
            return m_RendererID != 0 && otherRendererID != 0 && m_RendererID == otherRendererID;
        }

    private:
        std::string m_Path;
        uint32_t m_Width, m_Height;
        uint32_t m_RendererID = 0;

        int internalFormat = 0, dataFormat = 0;
        void *m_Data = nullptr;
    };
} // namespace Platform::OpenGL
