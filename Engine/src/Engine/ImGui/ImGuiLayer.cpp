#include "ImGuiLayer.h"
#include "Engine/Core/Application.h"
#include "Platform/OpenGL/imgui_impl_opengl3.h"
#include <glad/glad.h>
namespace Engine::MyImGui
{
    ImGuiLayer::~ImGuiLayer()
    {
    }

    void ImGuiLayer::OnAttach()
    {
        static int initialized = 0;
        initialized++;
        ENGINE_INFO("ImGuiLayer attached {0} times", initialized);
        // Initialize ImGui context here if needed
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGuiIO &io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
        ImGui_ImplOpenGL3_Init("#version 410 core");
    }

    void ImGuiLayer::OnDetach()
    {
        ENGINE_INFO("ImGuiLayer detached");
        // Cleanup ImGui context here if needed
        ImGui::DestroyContext();
    }

    void ImGuiLayer::OnUpdate()
    {
        Engine::Core::Application &app = Engine::Core::Application::Get();
        ImGuiIO &io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float)app.GetWindowWidth(), (float)app.GetWindowHeight());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        static bool show = true;
        ImGui::ShowDemoWindow(&show);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    bool ImGuiLayer::OnEvent(Engine::Event::Event &event)
    {
        Event::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<Event::MouseButtonPressedEvent>(BIND_EVENT_FN(ImGuiLayer::OnMouseButtonPressed));
        dispatcher.Dispatch<Event::MouseButtonReleasedEvent>(BIND_EVENT_FN(ImGuiLayer::OnMouseButtonReleased));
        dispatcher.Dispatch<Event::MouseMovedEvent>(BIND_EVENT_FN(ImGuiLayer::OnMouseMoved));
        dispatcher.Dispatch<Event::MouseScrolledEvent>(BIND_EVENT_FN(ImGuiLayer::OnMouseScrolled));
        dispatcher.Dispatch<Event::KeyPressedEvent>(BIND_EVENT_FN(ImGuiLayer::OnKeyPressed));
        dispatcher.Dispatch<Event::KeyReleasedEvent>(BIND_EVENT_FN(ImGuiLayer::OnKeyReleased));
        dispatcher.Dispatch<Event::KeyTypedEvent>(BIND_EVENT_FN(ImGuiLayer::OnKeyTyped));
        dispatcher.Dispatch<Event::WindowResizeEvent>(BIND_EVENT_FN(ImGuiLayer::OnWindowResize));
        return false;
    }

    bool ImGuiLayer::OnMouseButtonPressed(Engine::Event::MouseButtonPressedEvent &event)
    {
        ImGuiIO &io = ImGui::GetIO();
        io.MouseDown[event.GetMouseButton()] = true;
        return false;
    }

    bool ImGuiLayer::OnMouseButtonReleased(Engine::Event::MouseButtonReleasedEvent &event)
    {
        ImGuiIO &io = ImGui::GetIO();
        io.MouseDown[event.GetMouseButton()] = false;
        return false;
    }

    bool ImGuiLayer::OnMouseMoved(Engine::Event::MouseMovedEvent &event)
    {
        ImGuiIO &io = ImGui::GetIO();
        io.MousePos = ImVec2(event.GetX(), event.GetY());
        return false;
    }

    bool ImGuiLayer::OnMouseScrolled(Engine::Event::MouseScrolledEvent &event)
    {
        ImGuiIO &io = ImGui::GetIO();
        io.MouseWheel += event.GetYOffset();
        io.MouseWheelH += event.GetXOffset();
        return false;
    }

    bool ImGuiLayer::OnKeyPressed(Engine::Event::KeyPressedEvent &event)
    {
        // ImGuiIO &io = ImGui::GetIO();
        // io.AddKeyEvent(event.GetKeyCode(), true);
        return false;
    }

    bool ImGuiLayer::OnKeyReleased(Engine::Event::KeyReleasedEvent &event)
    {
        // ImGuiIO &io = ImGui::GetIO();
        // io.AddKeyEvent(event.GetKeyCode(), false);
        return false;
    }

    bool ImGuiLayer::OnKeyTyped(Engine::Event::KeyTypedEvent &event)
    {
        ImGuiIO &io = ImGui::GetIO();
        if (event.GetKeyCode() > 0 && event.GetKeyCode() < 0x10000)
            io.AddInputCharacter(event.GetKeyCode());
        return false;
    }

    bool ImGuiLayer::OnWindowResize(Engine::Event::WindowResizeEvent &event)
    {
        ImGuiIO &io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float)event.GetWidth(), (float)event.GetHeight());
        glViewport(0, 0, event.GetWidth(), event.GetHeight());
        return false;
    }

}