#include "ImGuiLayer.h"
#include "KeyMap.h"
#include "Engine/Core/Application.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
namespace Engine::MyImGui
{
    ImGuiLayer::~ImGuiLayer()
    {
    }

    void ImGuiLayer::OnAttach()
    {
        ENGINE_PROFILING_FUNC();
        ENGINE_INFO("ImGuiLayer attached");
        // Initialize ImGui context here if needed
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO &io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport

        io.Fonts->AddFontFromFileTTF("assets/fonts/googlesans/static/OpenSans-Bold.ttf", 18.0f);
        io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/googlesans/static/GoogleSans-Regular.ttf", 18.0f);

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        ImGuiStyle &style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        ENGINE_TRACE("Getting GLFWwindow from Application");

        Engine::Core::Application &app = Engine::Core::Application::Get();
        ENGINE_ASSERT(&app, "Application instance is null in ImGuiLayer::OnAttach()");
        GLFWwindow *window = static_cast<GLFWwindow *>(app.GetNativeWindow());
        ENGINE_ASSERT(window, "GLFWwindow is null in ImGuiLayer::OnAttach()");

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 410 core");
    }

    void ImGuiLayer::OnDetach()
    {
        ENGINE_PROFILING_FUNC();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    bool ImGuiLayer::OnEvent(Engine::Event::Event &event)
    {
        if (!m_BlockEvents)
            return false;
        ImGuiIO &io = ImGui::GetIO();
        event.Handled |= event.IsInCategory(Engine::Event::EventCategoryMouse) && io.WantCaptureMouse;
        event.Handled |= event.IsInCategory(Engine::Event::EventCategoryKeyboard) && io.WantCaptureKeyboard;
        return false;
    }

    void ImGuiLayer::Begin()
    {
        ENGINE_PROFILING_FUNC();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiLayer::End()
    {
        ENGINE_PROFILING_FUNC();
        ImGuiIO &io = ImGui::GetIO();

        io.DisplaySize = ImVec2((float)Engine::Core::Application::Get().GetWindowWidth(),
                                (float)Engine::Core::Application::Get().GetWindowHeight());

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }
}