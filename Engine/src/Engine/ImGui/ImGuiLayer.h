#include "Engine/Core/Layer.h"
#include "Engine/pcheader.h"
#include "Engine/Core/Core.h"
#include "Engine/Events/Event.h"
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
    };
}