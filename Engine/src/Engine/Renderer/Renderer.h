#pragma once

namespace Engine::Renderer
{
    enum class RendererAPI
    {
        None = 0,
        OpenGL = 1,
    };

    class Renderer
    {
    public:
        static inline RendererAPI GetAPI()
        {
            return CurrentRenderAPI;
        }

    private:
        static RendererAPI CurrentRenderAPI;
    };
} // namespace Engine::Renderer