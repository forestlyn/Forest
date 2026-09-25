#pragma once
#include "Engine/Core/Layer.h"
#include "Engine/Scene/Scene.h"
#include <filesystem>

namespace ForestRuntime
{
    class RuntimeLayer : public Engine::Core::Layer
    {
    public:
        RuntimeLayer(std::filesystem::path scenePath, uint32_t frameLimit);
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(Engine::Core::Timestep timestep) override;
        bool OnEvent(Engine::Event::Event &event) override;

    private:
        std::filesystem::path m_ScenePath;
        Engine::Ref<Engine::Scene> m_Scene;
        uint32_t m_FrameLimit = 0;
        uint32_t m_FrameCount = 0;
    };
}
