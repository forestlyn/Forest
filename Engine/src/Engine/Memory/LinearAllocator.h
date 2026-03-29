#pragma once
#include <cstdint>
#include <atomic>
#include <algorithm>
namespace Engine::Memory
{
    class LinearAllocator
    {
    public:
        LinearAllocator(size_t size);

        ~LinearAllocator();

        // 分配内存
        void *Allocate(size_t size, size_t alignment = 16);

        // 重置指针
        void Reset();

        LinearAllocator(const LinearAllocator &allocator) = delete;
        LinearAllocator &operator=(const LinearAllocator &allocator) = delete;

        LinearAllocator(LinearAllocator &&allocator) = delete;
        LinearAllocator &operator=(LinearAllocator &&allocator) = delete;

    private:
        uint8_t *m_Base;
        size_t m_Size;
        size_t m_Offset;
    };
}
