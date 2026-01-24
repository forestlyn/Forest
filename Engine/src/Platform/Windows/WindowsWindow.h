#pragma once
#include "Engine/pcheader.h"
#include "Engine/Core/Window.h"
#include "Engine/Core/Core.h"
#include "Engine/Renderer/RendererContext.h"
struct GLFWwindow;
namespace Platform::Windows
{
    class WindowsWindow : public Engine::Core::Window
    {
    public:
        WindowsWindow(const Engine::Core::WindowProps &props);
        virtual ~WindowsWindow();

        void OnUpdate() override;

        inline uint32_t GetWidth() const override { return m_Data.Width; }
        inline uint32_t GetHeight() const override { return m_Data.Height; }

        // Window attributes
        void SetEventCallback(const EventCallbackFn &callback) override;
        void SetVSync(bool enabled) override;
        bool IsVSync() const override;
        void SetFullScreen(bool enabled) override;
        bool IsFullscreen() const override;
        void Minimize() override;

        void *GetNativeWindow() const override { return m_Window; }

    private:
        virtual void Init(const Engine::Core::WindowProps &props);
        virtual void Shutdown();

        GLFWwindow *m_Window;
        Engine::Renderer::RendererContext *m_Context;

        struct WindowData
        {
            std::string Title;
            uint32_t Width, Height;
            bool VSync;
            bool Fullscreen;
            EventCallbackFn EventCallback;
        } m_Data;
    };
}
