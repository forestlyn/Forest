#include "ImGuiLayer.h"
// #include <imgui.h>
#include "Engine/Core/Application.h"
#include "Platform/OpenGL/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
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
        // Handle events if necessary
        return false;
    }
}