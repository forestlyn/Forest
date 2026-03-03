#pragma once
#include "Core.h"
#include <stdint.h>
#include <cstring>
namespace Engine::Core
{
    struct Buffer
    {
        uint8_t *Data;
        uint64_t Size;

        Buffer() : Data(nullptr), Size(0) {}
        Buffer(uint64_t size) : Data(new uint8_t[size]), Size(size) {}
        Buffer(const Buffer &other) : Data(new uint8_t[other.Size]), Size(other.Size)
        {
            memcpy(Data, other.Data, Size);
        }
        Buffer(Buffer &&other) noexcept : Data(other.Data), Size(other.Size)
        {
            other.Data = nullptr;
            other.Size = 0;
        }

        static Buffer Copy(Buffer other)
        {
            Buffer buffer(other.Size);
            memcpy(buffer.Data, other.Data, buffer.Size);
            return buffer;
        }

        void Allocate(uint64_t size)
        {
            if (Data)
                delete[] Data;
            Data = new uint8_t[size];
            Size = size;
        }

        void Release()
        {
            if (Data)
            {
                delete[] Data;
                Data = nullptr;
                Size = 0;
            }
        }

        template <typename T>
        T *As() { return reinterpret_cast<T *>(Data); }

        operator bool() const { return (bool)Data; }
    };

    struct ScopedBuffer
    {
    private:
        Buffer m_Buffer;

    public:
        ScopedBuffer() = default;
        ScopedBuffer(uint64_t size) : m_Buffer(size) {}
        ScopedBuffer(const Buffer &buffer) : m_Buffer(buffer) {}

        ~ScopedBuffer() { m_Buffer.Release(); }

        uint8_t *Data() { return m_Buffer.Data; }
        uint64_t Size() const { return m_Buffer.Size; }

        template <typename T>
        T *As() { return m_Buffer.As<T>(); }

        operator bool() const { return (bool)m_Buffer; }
    };
}