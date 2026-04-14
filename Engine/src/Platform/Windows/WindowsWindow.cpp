#include "WindowsWindow.h"
#include "Platform/OpenGL/OpenGLContext.h"
#include "glfw/glfw3.h"
#include "Engine/Profile/Instrumentor.h"
#include "Engine/Core/Application.h"
#ifdef FOREST_PLATFORM_WINDOWS
namespace Engine::Core
{
    Window *Window::Create(const WindowProps &props)
    {
        return new Platform::Windows::WindowsWindow(props);
    }
}
#endif

namespace Platform::Windows
{
    static bool s_GLFWInitialized = false;
    static uint32_t s_GLFWWindowCount = 0;

    WindowsWindow::WindowsWindow(const Engine::Core::WindowProps &props)
    {
        ENGINE_PROFILING_FUNC();
        Init(props);
    }

    WindowsWindow::~WindowsWindow()
    {
        ENGINE_PROFILING_FUNC();
        Shutdown();
    }

    void WindowsWindow::Init(const Engine::Core::WindowProps &props)
    {
        ENGINE_PROFILING_FUNC();
        m_Data.Title = props.Title;
        m_Data.Width = props.Width;
        m_Data.Height = props.Height;

        ENGINE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

        if (!s_GLFWInitialized)
        {
            ENGINE_PROFILING_SCOPE("glfwInit");

            int success = glfwInit();
            ENGINE_ASSERT(success, "Could not initialize GLFW!");
            s_GLFWInitialized = true;
        }

        {
            ENGINE_PROFILING_SCOPE("glfwCreateWindow");
            // RenderDoc requires an explicit core profile context.
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
            m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
            ENGINE_ASSERT(m_Window, "Failed to create GLFW window!");
            s_GLFWWindowCount++;
        }
        m_Context = new OpenGL::OpenGLContext(m_Window);

        ENQUEUE_RENDER_COMMAND(context = m_Context)
        context->Init();
        ENQUEUE_RENDER_COMMAND_END()

        glfwSetWindowUserPointer(m_Window, &m_Data);
        SetVSync(true);

        // Set GLFW callbacks

        // window events
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow *window, int width, int height)
                                  {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            data.Width = width;
            data.Height = height;

            Engine::Event::WindowResizeEvent event(width, height);
            data.EventCallback(event); });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *window)
                                   {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            Engine::Event::WindowCloseEvent event;
            data.EventCallback(event); });

        // key events
        glfwSetKeyCallback(m_Window, [](GLFWwindow *window, int key, int scancode, int action, int mods)
                           {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

            switch (action)
            {
            case GLFW_PRESS:
            {
                Engine::Event::KeyPressedEvent event(key, false);
                data.EventCallback(event);
                break;
            }
            case GLFW_RELEASE:
            {
                Engine::Event::KeyReleasedEvent event(key);
                data.EventCallback(event);
                break;
            }
            case GLFW_REPEAT:
            {
                Engine::Event::KeyPressedEvent event(key, true);
                data.EventCallback(event);
                break;
            }
            } });

        glfwSetCharCallback(m_Window, [](GLFWwindow *window, uint32_t keycode)
                            {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            Engine::Event::KeyTypedEvent event(keycode);
            data.EventCallback(event); });

        // mouse events
        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *window, int button, int action, int mods)
                                   {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            switch (action)
            {
            case GLFW_PRESS:
            {
                Engine::Event::MouseButtonPressedEvent event(button);
                data.EventCallback(event);
                break;
            }
            case GLFW_RELEASE:
            {
                Engine::Event::MouseButtonReleasedEvent event(button);
                data.EventCallback(event);
                break;
            }
            } });

        glfwSetScrollCallback(m_Window, [](GLFWwindow *window, double xoffset, double yoffset)
                              {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            Engine::Event::MouseScrolledEvent event((float)xoffset, (float)yoffset);
            data.EventCallback(event); });
        glfwSetCursorPosCallback(m_Window, [](GLFWwindow *window, double xpos, double ypos)
                                 {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            Engine::Event::MouseMovedEvent event((float)xpos, (float)ypos);
            data.EventCallback(event); });

        glfwSetWindowFocusCallback(m_Window, [](GLFWwindow *window, int focused)
                                   {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            if (focused == GLFW_TRUE)            {
                Engine::Event::WindowFocusEvent event;
                data.EventCallback(event);
            }
            else
            {
                Engine::Event::WindowLostFocusEvent event;
                data.EventCallback(event);
            } });
    }

    void WindowsWindow::Shutdown()
    {
        ENGINE_PROFILING_FUNC();
        if (m_Context)
        {
            auto *context = m_Context;
            m_Context = nullptr;

            ENQUEUE_RENDER_COMMAND(context)
            context->Cleanup();
            delete context;

            // 【极其关键的一步】：必须在渲染线程解绑 Context！
            // 这样主线程后续销毁 Window 时才不会与渲染线程冲突
            glfwMakeContextCurrent(nullptr);

            ENQUEUE_RENDER_COMMAND_END()

            // 阻塞主线程，严格等待渲染线程完成清理和解绑
            Engine::Core::Application::Get().FlushRendererCommands();
        }

        if (m_Window)
        {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;

            ENGINE_ASSERT(s_GLFWWindowCount > 0, "GLFW window count underflow!");
            s_GLFWWindowCount--;
        }

        if (s_GLFWInitialized && s_GLFWWindowCount == 0)
        {
            glfwTerminate();
            s_GLFWInitialized = false;
        }
    }

    void WindowsWindow::OnUpdateEvent()
    {
        glfwPollEvents();
    }

    void WindowsWindow::OnUpdate()
    {
        ENGINE_PROFILING_FUNC();
        ENQUEUE_RENDER_COMMAND(context = m_Context)
        context->SwapBuffers();
        ENQUEUE_RENDER_COMMAND_END()
    }

    void WindowsWindow::SetEventCallback(const EventCallbackFn &callback)
    {
        m_Data.EventCallback = callback;
    }
    void WindowsWindow::SetVSync(bool enabled)
    {
        ENQUEUE_RENDER_COMMAND(context = m_Context, enabled)
        context->SetVSync(enabled);
        ENQUEUE_RENDER_COMMAND_END()

        m_Data.VSync = enabled;
    }

    bool WindowsWindow::IsVSync() const
    {
        return m_Data.VSync;
    }
    void WindowsWindow::SetFullScreen(bool enabled)
    {
        if (enabled == m_Data.Fullscreen)
            return;

        if (enabled)
        {
            GLFWmonitor *monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(m_Window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
        else
        {
            glfwSetWindowMonitor(m_Window, nullptr, 100, 100, m_Data.Width, m_Data.Height, 0);
        }
        m_Data.Fullscreen = enabled;
    }

    bool WindowsWindow::IsFullscreen() const
    {
        return m_Data.Fullscreen;
    }
    void WindowsWindow::Minimize()
    {
        glfwIconifyWindow(m_Window);
    }
}
