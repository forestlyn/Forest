#pragma once
#include "Engine/pcheader.h"

namespace Engine::Renderer
{
    class RendererContext
    {
    public:
        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;
        virtual void Cleanup() = 0;
        virtual void SetVSync(bool enabled) = 0;
    };
}