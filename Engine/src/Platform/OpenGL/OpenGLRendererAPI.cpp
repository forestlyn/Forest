#include "OpenGLRendererAPI.h"

#include <glad/glad.h>
#include "Engine/Profile/Instrumentor.h"

namespace Engine::Platform::OpenGL
{
    void OpenGLRendererAPI::BeginOverlay()
    {
        OverlayState state{};
        state.DepthTest = glIsEnabled(GL_DEPTH_TEST);
        GLboolean depthWrite;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWrite);
        state.DepthWrite = depthWrite;
        state.Blend = glIsEnabled(GL_BLEND);
        state.Cull = glIsEnabled(GL_CULL_FACE);
        state.Scissor = glIsEnabled(GL_SCISSOR_TEST);
        glGetIntegerv(GL_BLEND_SRC_RGB, &state.SrcRGB);
        glGetIntegerv(GL_BLEND_DST_RGB, &state.DstRGB);
        glGetIntegerv(GL_BLEND_SRC_ALPHA, &state.SrcAlpha);
        glGetIntegerv(GL_BLEND_DST_ALPHA, &state.DstAlpha);
        glGetIntegerv(GL_BLEND_EQUATION_RGB, &state.EquationRGB);
        glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &state.EquationAlpha);
        m_OverlayStates.push_back(state);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);
        glDisable(GL_SCISSOR_TEST);
        glEnable(GL_BLEND);
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }

    void OpenGLRendererAPI::EndOverlay()
    {
        ENGINE_ASSERT(!m_OverlayStates.empty(), "Unbalanced overlay render pass");
        if (m_OverlayStates.empty()) return;
        const auto state = m_OverlayStates.back();
        m_OverlayStates.pop_back();
        const auto restore = [](GLenum cap, bool enabled) { if (enabled) glEnable(cap); else glDisable(cap); };
        restore(GL_DEPTH_TEST, state.DepthTest);
        glDepthMask(state.DepthWrite ? GL_TRUE : GL_FALSE);
        restore(GL_BLEND, state.Blend);
        restore(GL_CULL_FACE, state.Cull);
        restore(GL_SCISSOR_TEST, state.Scissor);
        glBlendEquationSeparate(state.EquationRGB, state.EquationAlpha);
        glBlendFuncSeparate(state.SrcRGB, state.DstRGB, state.SrcAlpha, state.DstAlpha);
    }

    void OpenGLRendererAPI::Init()
    {
        ENGINE_PROFILING_FUNC();

        ENGINE_INFO("Initializing OpenGL Renderer API");
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
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

    void OpenGLRendererAPI::DrawIndexed(const Ref<Renderer::VertexArray> &vertexArray, uint32_t indexCount)
    {
        ENGINE_PROFILING_FUNC();
        vertexArray->Bind();
        uint32_t count = indexCount == -1 ? vertexArray->GetIndexBuffer()->GetCount() : indexCount;
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
    }
    void OpenGLRendererAPI::DrawLine(const Ref<Renderer::VertexArray> &vertexArray, uint32_t vertexCount)
    {
        ENGINE_PROFILING_FUNC();
        vertexArray->Bind();
        glDrawArrays(GL_LINES, 0, vertexCount);
    }
    void OpenGLRendererAPI::SetLineWidth(float width)
    {
        glLineWidth(width);
    }
} // namespace Engine::Platform::OpenGL
