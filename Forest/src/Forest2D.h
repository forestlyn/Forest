#pragma once
#include "Engine.h"
#include <glm/glm.hpp>
class Forest2D : public Engine::Core::Layer
{
public:
    Forest2D(const std::string &name = "Forest2DLayer");
    virtual ~Forest2D();

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(Engine::Core::Timestep timestep) override;
    virtual void OnImGuiRender() override;
    virtual bool OnEvent(Engine::Event::Event &event) override;

private:
    Engine::Ref<Engine::Renderer::VertexArray> m_SquareVA;
    Engine::Ref<Engine::Renderer::Shader> m_FlatColorShader;
    Engine::OrthographicCameraController m_CameraController;

    glm::vec4 m_SquareColor = {0.2f, 0.3f, 0.8f, 1.0f};
};
