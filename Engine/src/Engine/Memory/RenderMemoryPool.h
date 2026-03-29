#pragma once
#include "LinearAllocator.h"

namespace Engine::Memory
{
    class RenderMemoryPool
    {
    public:
        RenderMemoryPool(size_t size);
        ~RenderMemoryPool();

        void *Allocate(size_t size);

        void Swap();

        LinearAllocator *GetRendererAllocator();

        RenderMemoryPool(const RenderMemoryPool &) = delete;
        RenderMemoryPool &operator=(const RenderMemoryPool &) = delete;

        RenderMemoryPool(RenderMemoryPool &&other) = delete;
        RenderMemoryPool &operator=(RenderMemoryPool &&other) = delete;

    private:
        LinearAllocator *m_Allocator[2];
        int m_LogicIndex;
    };
}