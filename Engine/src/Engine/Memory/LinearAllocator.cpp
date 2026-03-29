#include "LinearAllocator.h"

namespace Engine::Memory
{
    LinearAllocator::LinearAllocator(size_t size) : m_Size(size)
    {
        m_Base = new uint8_t[size];
        m_Offset = 0;
    }

    LinearAllocator::~LinearAllocator()
    {
        delete[] m_Base;
    }

    void *LinearAllocator::Allocate(size_t size, size_t alignment)
    {
        // 1. 处理对齐
        size_t alignedOffset = (m_Offset + alignment - 1) & ~(alignment - 1);

        // 2. 检查溢出
        if (alignedOffset + size > m_Size)
        {
            return nullptr; // 或者抛出异常，或使用备用 malloc
        }

        // 3. 更新offset
        m_Offset = alignedOffset + size;

        return m_Base + alignedOffset;
    }

    void LinearAllocator::Reset()
    {
        m_Offset = 0;
    }

} // namespace Engine::Memory
