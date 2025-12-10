#include "Forest2D.h"
#include "imgui.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
Forest2D::Forest2D(const std::string &name)
    : Layer(name), m_CameraController(Engine::OrthographicCameraController(16.0f / 9.0f, 1.0f, true))
{
    float vertices[4 * 3] = {
        -0.5f, -0.5f, -50.0f,
        -0.5f, 0.5f, -50.0f,
        0.5f, 0.5f, -50.0f,
        0.5f, -0.5f, -50.0f};

    Engine::Renderer::BufferLayout layout = {
        {Engine::Renderer::ShaderDataType::Float3, "a_Position"}};

    m_SquareVA = Engine::Renderer::VertexArray::Create();
    Engine::Ref<Engine::Renderer::VertexBuffer> vertexBuffer = Engine::Renderer::VertexBuffer::Create(vertices, sizeof(vertices));
    vertexBuffer->SetLayout(layout);
    m_SquareVA->AddVertexBuffer(vertexBuffer);
    uint32_t indices[6] = {0, 1, 2, 2, 3, 0};
    Engine::Ref<Engine::Renderer::IndexBuffer> indexBuffer =
        Engine::Ref<Engine::Renderer::IndexBuffer>(
            Engine::Renderer::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
    m_SquareVA->SetIndexBuffer(indexBuffer);
    m_FlatColorShader = Engine::Renderer::Shader::Create("assets/shaders/FlatColorShader.glsl");
}

Forest2D::~Forest2D()
{
}

void Forest2D::OnAttach()
{
    LOG_INFO("Forest2D Layer attached!");
}

void Forest2D::OnDetach()
{
    LOG_INFO("Forest2D Layer detached!");
}

void Forest2D::OnUpdate(Engine::Core::Timestep timestep)
{
    m_CameraController.OnUpdate(timestep);

    Engine::Renderer::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
    Engine::Renderer::RenderCommand::Clear();

    Engine::Renderer::Renderer::BeginScene(m_CameraController.GetCamera());
    m_FlatColorShader->Bind();
    m_FlatColorShader->SetFloat4("u_Color", m_SquareColor);
    Engine::Renderer::Renderer::Submit(m_FlatColorShader, m_SquareVA,
                                       glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) *
                                           glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f)));

    Engine::Renderer::Renderer::EndScene();
}
void Forest2D::OnImGuiRender()
{
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
    ImGui::End();
}

bool Forest2D::OnEvent(Engine::Event::Event &event)
{
    m_CameraController.OnEvent(event);
    return false;
}
