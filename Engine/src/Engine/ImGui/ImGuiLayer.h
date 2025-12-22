#pragma once
#include "Engine/Core/Layer.h"
#include "Engine/pcheader.h"
#include "Engine/Core/Core.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Events/WindowEvent.h"
namespace Engine::MyImGui
{
    class FOREST_API ImGuiLayer : public Engine::Core::Layer
    {
    public:
        ImGuiLayer()
            : Layer("ImGuiLayer")
        {
        }

        ~ImGuiLayer() override;

        void OnAttach() override;
        void OnDetach() override;

        void Begin();
        void End();

        void OpenDockSpace();
    };
}