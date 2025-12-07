#pragma once
#include "Shader/VertexArray.h"
#include "glm/vec4.hpp"
namespace Engine::Renderer
{

    class RendererAPI
    {
    public:
        enum class API
        {
            None = 0,
            OpenGL = 1,
        };

    public:
        virtual void Init() = 0;
        virtual void SetClearColor(const glm::vec4 &color) = 0;
        virtual void Clear() = 0;

        virtual void DrawIndexed(Ref<VertexArray> &vertexArray) = 0;
        static API GetAPI()
        {
            return CurrentAPI;
        }

    private:
        static API CurrentAPI;
    };

}