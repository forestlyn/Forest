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
        void OnUpdate() override;
        bool OnEvent(Engine::Event::Event &event) override;

    private:
        bool OnMouseButtonPressed(Engine::Event::MouseButtonPressedEvent &event);
        bool OnMouseButtonReleased(Engine::Event::MouseButtonReleasedEvent &event);
        bool OnMouseMoved(Engine::Event::MouseMovedEvent &event);
        bool OnMouseScrolled(Engine::Event::MouseScrolledEvent &event);

        bool OnKeyPressed(Engine::Event::KeyPressedEvent &event);
        bool OnKeyReleased(Engine::Event::KeyReleasedEvent &event);
        bool OnKeyTyped(Engine::Event::KeyTypedEvent &event);

        bool OnWindowResize(Engine::Event::WindowResizeEvent &event);
    };
}