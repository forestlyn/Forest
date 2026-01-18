#include "UUID.h"
#include <random>
namespace Engine
{
    static std::random_device rd;
    static std::mt19937_64 eng(rd());
    static std::uniform_int_distribution<uint64_t> distr;
    UUID::UUID() : m_UUID(distr(eng))
    {
    }

    UUID::UUID(uint64_t uuid)
        : m_UUID(uuid)
    {
    }
}