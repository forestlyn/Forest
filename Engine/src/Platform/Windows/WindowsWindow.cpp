#include "WindowsWindow.h"
#include <glfw/glfw3.h>
#include <glad/glad.h>
#ifdef FOREST_PLATFORM_WINDOWS
namespace Engine::Core
{
    Window *Window::Create(const WindowProps &props)
    {
        return new Engine::Platform::Windows::WindowsWindow(props);
    }
}
#endif

namespace Engine::Platform::Windows
{
    static bool s_GLFWInitialized = false;

    WindowsWindow::WindowsWindow(const Core::WindowProps &props)
    {
        Init(props);
    }

    WindowsWindow::~WindowsWindow()
    {
        Shutdown();
    }

    void WindowsWindow::Init(const Core::WindowProps &props)
    {
        m_Data.Title = props.Title;
        m_Data.Width = props.Width;
        m_Data.Height = props.Height;

        ENGINE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

        if (!s_GLFWInitialized)
        {
            int success = glfwInit();
            ENGINE_ASSERT(success, "Could not initialize GLFW!");
            s_GLFWInitialized = true;
        }

        m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
        glfwMakeContextCurrent(m_Window);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        ENGINE_ASSERT(status, "Failed to initialize Glad!");
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
    }

    void WindowsWindow::Shutdown()
    {
        glfwDestroyWindow(m_Window);
    }

    void WindowsWindow::OnUpdate()
    {
        glfwPollEvents();
        glfwSwapBuffers(m_Window);
    }

    void WindowsWindow::SetEventCallback(const EventCallbackFn &callback)
    {
        m_Data.EventCallback = callback;
    }

    void WindowsWindow::SetVSync(bool enabled)
    {
        if (enabled)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);

        m_Data.VSync = enabled;
    }

    bool WindowsWindow::IsVSync() const
    {
        return m_Data.VSync;
    }
}