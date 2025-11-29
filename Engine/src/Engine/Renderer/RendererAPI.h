#pragma once
#include "VertexArray.h"
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
        virtual void SetClearColor(const glm::vec4 &color) = 0;
        virtual void Clear() = 0;

        virtual void DrawIndexed(std::shared_ptr<VertexArray> &vertexArray) = 0;
        static API GetAPI()
        {
            return CurrentAPI;
        }

    private:
        static API CurrentAPI;
    };

}