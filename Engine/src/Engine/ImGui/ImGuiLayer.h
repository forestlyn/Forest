#pragma once
#include "Engine/Core/Layer.h"
#include "Engine/pcheader.h"
#include "Engine/Core/Core.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Events/WindowEvent.h"
namespace Engine::MyImGui
{
    class ImGuiLayer : public Engine::Core::Layer
    {
    public:
        ImGuiLayer()
            : Layer("ImGuiLayer")
        {
        }

        ~ImGuiLayer() override;

        void OnAttach() override;
        void OnDetach() override;
        bool OnEvent(Engine::Event::Event &event) override;

        void Begin();
        void End();

        void BlockEvents(bool block) { m_BlockEvents = block; }

        void SetDarkThemeColors();

    private:
        bool m_BlockEvents = true;
    };
}