#include "WindowsInput.h"
#include <glfw/glfw3.h>
#include "Engine/Core/Application.h"

#ifdef FOREST_PLATFORM_WINDOWS
namespace Engine::Core
{
    Input *Input::s_Instance = new Platform::Windows::WindowsInput();
}
#endif

namespace Engine::Platform::Windows
{

    bool WindowsInput::IsKeyPressedImpl(int keycode)
    {
        GLFWwindow *window = static_cast<GLFWwindow *>(Engine::Core::Application::Get().GetWindow().GetNativeWindow());
        int state = glfwGetKey(window, keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool WindowsInput::IsMouseButtonPressedImpl(int button)
    {
        GLFWwindow *window = static_cast<GLFWwindow *>(Engine::Core::Application::Get().GetWindow().GetNativeWindow());
        int state = glfwGetMouseButton(window, button);
        return state == GLFW_PRESS;
    }

    Engine::Math::Vector2 WindowsInput::GetMousePositionImpl()
    {
        GLFWwindow *window = static_cast<GLFWwindow *>(Engine::Core::Application::Get().GetWindow().GetNativeWindow());
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        return {static_cast<float>(xpos), static_cast<float>(ypos)};
    }

    float WindowsInput::GetMouseXImpl()
    {
        return GetMousePositionImpl().x;
    }

    float WindowsInput::GetMouseYImpl()
    {
        return GetMousePositionImpl().y;
    }
}