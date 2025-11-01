#pragma once
#include "Engine/pcheader.h"
#include "Engine/Core/Window.h"
#include "Engine/Core/Core.h"
struct GLFWwindow;
namespace Engine::Platform::Windows
{
    class WindowsWindow : public Core::Window
    {
    public:
        WindowsWindow(const Core::WindowProps &props);
        virtual ~WindowsWindow();

        void OnUpdate() override;

        inline uint32_t GetWidth() const override { return m_Data.Width; }
        inline uint32_t GetHeight() const override { return m_Data.Height; }

        // Window attributes
        void SetEventCallback(const EventCallbackFn &callback) override;
        void SetVSync(bool enabled) override;
        bool IsVSync() const override;

        void *GetNativeWindow() const override { return m_Window; }

    private:
        virtual void Init(const Core::WindowProps &props);
        virtual void Shutdown();

        GLFWwindow *m_Window;

        struct WindowData
        {
            std::string Title;
            uint32_t Width, Height;
            bool VSync;

            EventCallbackFn EventCallback;
        } m_Data;
    };
}
