#pragma once
#include "Engine/Renderer/RendererContext.h"

struct GLFWwindow;

namespace Platform::OpenGL
{
    class OpenGLContext : public Engine::Renderer::RendererContext
    {
    public:
        OpenGLContext(GLFWwindow *windowHandle);

        virtual void Init() override;
        virtual void SwapBuffers() override;

    private:
        GLFWwindow *m_WindowHandle;
    };
} // namespace
