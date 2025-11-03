#pragma once
#include "Core.h"
#include "Engine/pcheader.h"
#include "Engine/Events/Event.h"
namespace Engine::Core
{
    class FOREST_API Layer
    {
    public:
        Layer(const std::string &name = "Layer");
        virtual ~Layer();

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate() {}
        virtual void OnImGuiRender() {}
        virtual bool OnEvent(Engine::Event::Event &event) { return false; }
        inline const std::string &GetName() const { return m_DebugName; }

    protected:
        std::string m_DebugName;
    };
}