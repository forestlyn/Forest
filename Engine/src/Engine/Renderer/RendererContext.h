#pragma once
#include "Engine/pcheader.h"

namespace Engine::Renderer
{
    class RendererContext
    {
    public:
        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;
    };
}