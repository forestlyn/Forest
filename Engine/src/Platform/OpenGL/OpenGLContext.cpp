#include "OpenGLContext.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Engine/pcheader.h"
namespace Platform::OpenGL
{
    OpenGLContext::OpenGLContext(GLFWwindow *windowHandle)
        : m_WindowHandle(windowHandle)
    {
        ENGINE_ASSERT(windowHandle, "Window handle is null!");
    }

    void OpenGLContext::Init()
    {
        glfwMakeContextCurrent(m_WindowHandle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        ENGINE_ASSERT(status, "Failed to initialize Glad!");

        ENGINE_INFO("OpenGL Info:");
        ENGINE_INFO("  Vendor: {0}", reinterpret_cast<const char *>(glGetString(GL_VENDOR)));
        ENGINE_INFO("  Renderer: {0}", reinterpret_cast<const char *>(glGetString(GL_RENDERER)));
        ENGINE_INFO("  Version: {0}", reinterpret_cast<const char *>(glGetString(GL_VERSION)));
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_WindowHandle);
    }

    void OpenGLContext::SetVSync(bool enabled)
    {
        if (enabled)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);
    }

    void OpenGLContext::Cleanup()
    {
        // OpenGL specific cleanup code can be added here if needed
    }
} // namespace Platform::OpenGL