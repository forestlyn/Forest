#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Engine::Platform::OpenGL
{
    void OpenGLRendererAPI::Init()
    {
        ENGINE_INFO("Initializing OpenGL Renderer API");
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void OpenGLRendererAPI::SetClearColor(const glm::vec4 &color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void OpenGLRendererAPI::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glViewport(x, y, width, height);
        // ENGINE_INFO("Viewport set to x:{0}, y:{1}, width:{2}, height:{3}", x, y, width, height);
    }

    void OpenGLRendererAPI::DrawIndexed(Ref<Renderer::VertexArray> &vertexArray)
    {
        glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
    }
} // namespace Engine::Platform::OpenGL