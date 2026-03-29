#include "RenderMemoryPool.h"
namespace Engine::Memory
{
    RenderMemoryPool::RenderMemoryPool(size_t size)
    {
        m_Allocator[0] = new LinearAllocator(size);
        m_Allocator[1] = new LinearAllocator(size);
        m_LogicIndex = 0;
    }

    RenderMemoryPool::~RenderMemoryPool()
    {
        delete m_Allocator[0];
        delete m_Allocator[1];
        m_Allocator[0] = nullptr;
        m_Allocator[1] = nullptr;
    }

    void *RenderMemoryPool::Allocate(size_t size)
    {
        return m_Allocator[m_LogicIndex]->Allocate(size);
    }

    void RenderMemoryPool::Swap()
    {
        m_LogicIndex = 1 - m_LogicIndex;
        // 重置即将被逻辑层写入的那个 Pool
        m_Allocator[m_LogicIndex]->Reset();
    }

    LinearAllocator *RenderMemoryPool::GetRendererAllocator()
    {
        return m_Allocator[1 - m_LogicIndex];
    }
}
