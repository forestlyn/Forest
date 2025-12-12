#pragma once
#include "Timer.h"
#include "Engine/Core/Layer.h"
#include <vector>

namespace Engine::Profile
{
    class ProfileLayer : public Engine::Core::Layer
    {
    public:
        ProfileLayer();
        ~ProfileLayer() override;
        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnImGuiRender() override;

    public:
        static std::vector<ProfileResult> ProfileResults;
    };
} // namespace Engine::Profile