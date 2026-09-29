#include "RuntimeLayer.h"
#include "Engine/Core/Application.h"
#include "Engine/Renderer/RenderCommand.h"
#include "Engine/Serialization/SceneSerialize.h"
#include "Engine/Scripts/ScriptEngine.h"
#include <stdexcept>

namespace ForestRuntime
{
    RuntimeLayer::RuntimeLayer(std::filesystem::path scenePath, uint32_t frameLimit)
        : Layer("Runtime"), m_ScenePath(std::move(scenePath)), m_FrameLimit(frameLimit) {}

    void RuntimeLayer::OnAttach()
    {
        m_Scene = Engine::CreateRef<Engine::Scene>();
        Engine::Serialization::SceneSerialize serializer(m_Scene);
        if (!serializer.Deserialize(m_ScenePath.string()))
            throw std::runtime_error("Failed to load scene: " + m_ScenePath.string());

        auto &registry = m_Scene->GetRegistry();
        bool hasCamera = false;
        for (auto entity : registry.view<Engine::CameraComponent, Engine::TransformComponent>())
            hasCamera |= registry.get<Engine::CameraComponent>(entity).Primary;
        if (!hasCamera && !m_Scene->HasValidCanvas())
            throw std::runtime_error("Startup scene requires a primary camera or a valid UI Canvas");

        for (auto entity : registry.view<Engine::ScriptComponent>())
        {
            const auto &name = registry.get<Engine::ScriptComponent>(entity).ScriptClassName;
            if (!name.empty() && !Engine::ScriptEngine::EntityClassExists(name))
                throw std::runtime_error("Missing script class: " + name +
                    ". Configure ScriptAssembly or pass --script-assembly <dll>.");
        }

        auto &app = Engine::Core::Application::Get();
        const auto [width, height] = app.GetWindow().GetFramebufferSize();
        m_Scene->SetViewportSize(width, height);
        Engine::Renderer::RenderCommand::SetViewport(0, 0, width, height);
        m_Scene->OnRuntimeStart();
        ENGINE_INFO("Runtime started: {}", m_ScenePath.string());
    }

    void RuntimeLayer::OnDetach()
    {
        if (m_Scene)
        {
            m_Scene->OnRuntimeStop();
            m_Scene.reset();
            ENGINE_INFO("Runtime stopped after {} frames", m_FrameCount);
        }
    }

    void RuntimeLayer::OnUpdate(Engine::Core::Timestep timestep)
    {
        // Query pixels each frame: monitor DPI changes need not change logical window size.
        const auto [width, height] = Engine::Core::Application::Get().GetWindow().GetFramebufferSize();
        m_Scene->SetViewportSize(width, height);
        if (!width || !height) return;
        Engine::Renderer::RenderCommand::SetViewport(0, 0, width, height);
        Engine::Renderer::RenderCommand::SetClearColor({0.08f, 0.08f, 0.1f, 1.0f});
        Engine::Renderer::RenderCommand::Clear();
        m_Scene->OnUpdateRuntime(timestep);
        ++m_FrameCount;
        if (m_FrameLimit && m_FrameCount >= m_FrameLimit)
            Engine::Core::Application::Get().Shutdown();
    }

    bool RuntimeLayer::OnEvent(Engine::Event::Event &event)
    {
        Engine::Event::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<Engine::Event::WindowResizeEvent>([this](auto &resize)
        {
            if (m_Scene)
            {
                const auto [width, height] = Engine::Core::Application::Get().GetWindow().GetFramebufferSize();
                m_Scene->SetViewportSize(resize.GetWidth() && resize.GetHeight() ? width : 0,
                                         resize.GetWidth() && resize.GetHeight() ? height : 0);
            }
            return false;
        });
        return false;
    }
}
