#include "Random.h"

namespace Engine::Math::Random
{
    std::mt19937 s_RandomEngine(std::random_device{}());
    std::uniform_int_distribution<std::mt19937::result_type> s_Distribution(0, std::numeric_limits<uint32_t>::max());
} // namespace Engine::Math::Random